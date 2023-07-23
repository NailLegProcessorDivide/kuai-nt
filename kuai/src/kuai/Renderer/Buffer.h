#pragma once

namespace kuai {

    enum class ShaderDataType 
    {
        NONE = 0, INT, FLOAT, VEC2, VEC3, VEC4, MAT3, MAT4
    };

    enum class DrawHint
    {
        STATIC, DYNAMIC
    };

    static u32 sizeOfShaderDataType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::INT:   return 4;
            case ShaderDataType::FLOAT: return 4;
            case ShaderDataType::VEC2:  return 8;
            case ShaderDataType::VEC3:  return 12;
            case ShaderDataType::VEC4:  return 16;
            case ShaderDataType::MAT3:  return 36;
            case ShaderDataType::MAT4:  return 64;
        }

        KU_CORE_ASSERT(false, "Unknown shader data type.");
        return 0;
    }

    struct BufferElement
    {
        std::string name;
        ShaderDataType type;
        u32 size;
        u32 offset;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name) : 
            name(name), type(type), size(sizeOfShaderDataType(type)), offset(0) {}

        u32 getComponentCount() const
        { 
            switch (type)
            {
                case ShaderDataType::INT:   return 1;
                case ShaderDataType::FLOAT: return 1;
                case ShaderDataType::VEC2:  return 2;
                case ShaderDataType::VEC3:  return 3;
                case ShaderDataType::VEC4:  return 4;
                case ShaderDataType::MAT3:  return 3; // (3 * Vec3)
                case ShaderDataType::MAT4:  return 4; // (4 * Vec4)
            }

            return 0;
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(std::initializer_list<BufferElement> elements) :
            elements(elements)
        {
            calcOffsetsAndStride();
        }

        u32 getStride() const { return stride; }
        const std::vector<BufferElement>& getElements() const { return elements; }

        std::vector<BufferElement>::iterator begin() { return elements.begin(); }
        std::vector<BufferElement>::iterator end() { return elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return elements.end(); }

    private:
        void calcOffsetsAndStride()
        {
            u32 offset = 0;
            stride = 0;
            for (auto& element : elements)
            {
                element.offset = offset;
                offset += element.size;
                stride += element.size;
            }
        }

    private:
        std::vector<BufferElement> elements;
        u32 stride = 0;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(u32 size);
        VertexBuffer(const float* vertices, u32 size, DrawHint drawHint = DrawHint::STATIC);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;

        void setData(const void* data, u32 size);
        void reset(const void* data, u32 size, DrawHint drawHint = DrawHint::STATIC);

        const BufferLayout& getLayout() const { return layout; }
        void setLayout(const BufferLayout& layout) { this->layout = layout; }

    private:
        u32 bufId;
        BufferLayout layout;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(const u32* indices, u32 count);
        ~IndexBuffer();

        void bind() const;
        void unbind() const;

        u32 getCount() const { return count; }

    private:
        u32 bufId;
        u32 count;
    };
 
    struct IndirectCommand
    {
        u32 count;          // Number of elements to be drawn per instance
        u32 instanceCount;  // Number of instances
        u32 firstIndex;     // Offset of mesh in index buffer
        i32 baseVertex;     // Offset of mesh in vertex buffer
        u32 baseInstance;   // First instanced model index
    };
             
    class IndirectBuffer
    {
    public:
        IndirectBuffer(const std::vector<IndirectCommand>& commands);
        ~IndirectBuffer();

        void bind() const;
        void unbind() const;

        u32 getCount() const { return count; }

    private:
        u32 bufId;
        u32 count;
    };

    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void bind() const;
        void unbind() const;

        const std::vector<Rc<VertexBuffer>>& getVertexBuffers() const;
        void addVertexBuffer(Rc<VertexBuffer> buf);

        Rc<IndexBuffer> getIndexBuffer() const;
        void setIndexBuffer(Rc<IndexBuffer> buf);

    private:
        std::vector<Rc<VertexBuffer>> vertexBufs;
        Rc<IndexBuffer> indexBuf;
        u32 vaoId;
        u32 index = 0;
    };
}