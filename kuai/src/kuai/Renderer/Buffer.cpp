#include "kpch.h"

#include "Buffer.h"

#include "glad/glad.h"

namespace kuai {

	static GLenum getOpenGLType(ShaderDataType type)
	{
		switch(type)
		{
			case ShaderDataType::INT:
				return GL_INT;
			case ShaderDataType::FLOAT:
			case ShaderDataType::VEC2:
			case ShaderDataType::VEC3:
			case ShaderDataType::VEC4:
			case ShaderDataType::MAT3:
			case ShaderDataType::MAT4:
				return GL_FLOAT;
		}

		KU_CORE_ASSERT(false, "Unknown shader data type.");
		return 0;
	}

	// Vertex Buffer *********************************************************

	VertexBuffer::VertexBuffer(u32 size)
	{
		glCreateBuffers(1, &bufId);
		glBindBuffer(GL_ARRAY_BUFFER, bufId);
		glBufferData(bufId, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(const float* vertices, u32 size, DrawHint drawHint)
	{
		glCreateBuffers(1, &bufId);
		glBindBuffer(GL_ARRAY_BUFFER, bufId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawHint == DrawHint::STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &bufId);
	}

	void VertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufId);
	}

	void VertexBuffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::setData(const void* data, u32 size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void VertexBuffer::reset(const void* data, u32 size, DrawHint drawHint)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufId);
		glBufferData(GL_ARRAY_BUFFER, size, data, drawHint == DrawHint::STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	// Index Buffer ***********************************************************

	IndexBuffer::IndexBuffer(const u32* indices, u32 count) : count(count)
	{
		glCreateBuffers(1, &bufId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * count, indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &bufId);
	}

	void IndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufId);
	}

	void IndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Indirect Buffer ********************************************************

	IndirectBuffer::IndirectBuffer(const std::vector<IndirectCommand>& commands)
	{
		glCreateBuffers(1, &bufId);
		count = commands.size();
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufId);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectCommand) * count, commands.data(), GL_STATIC_DRAW);
	}

	IndirectBuffer::~IndirectBuffer()
	{
		glDeleteBuffers(1, &bufId);
	}

	void IndirectBuffer::bind() const
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufId);
	}

	void IndirectBuffer::unbind() const
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}

	// Vertex Array ***********************************************************

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &vaoId);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &vaoId);
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(vaoId);
	}

	void VertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	const std::vector<Rc<VertexBuffer>>& VertexArray::getVertexBuffers() const
	{
		return vertexBufs;
	}

	void VertexArray::addVertexBuffer(Rc<VertexBuffer> buf)
	{
		KU_CORE_ASSERT(buf->getLayout().getElements().size(), "Vertex buffer has no layout.");

		glBindVertexArray(vaoId);
		buf->bind();

		auto& layout = buf->getLayout();
		for (auto& element : layout)
		{
			switch (element.type)
			{
				case ShaderDataType::INT:
				{
					glEnableVertexAttribArray(index);
					glVertexAttribIPointer(
						index,
						element.getComponentCount(),
						GL_INT,
						layout.getStride(),
						(const void*)element.offset);
					glVertexAttribDivisor(index, 1); // Tells vertex attribute to increment once per instance instead of per vertex
					index++;
					break;
				}
				case ShaderDataType::FLOAT:
					glVertexAttribDivisor(index, 1);
				case ShaderDataType::VEC2:
				case ShaderDataType::VEC3:
				case ShaderDataType::VEC4:
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(
						index,
						element.getComponentCount(),
						GL_FLOAT,
						GL_FALSE,
						layout.getStride(),
						(const void*)element.offset
					);
					index++;
					break;
				}
				case ShaderDataType::MAT3:
				case ShaderDataType::MAT4:
				{
					// The maximum size of a vertex attribute is 4 elements (vec4), so need to assign mutiple vertex attribute slots for a matrix
					u8 count = element.getComponentCount();
					for (u8 i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(index);
						glVertexAttribPointer(
							index,
							count,
							GL_FLOAT,
							GL_FALSE,
							layout.getStride(),
							(const void*)(element.offset + sizeof(float) * count * i)
						);
						glVertexAttribDivisor(index, 1);
						index++;
					}
				}
			}
		}

		vertexBufs.push_back(buf);
	}

	Rc<IndexBuffer> VertexArray::getIndexBuffer() const
	{
		return indexBuf;
	}

	void VertexArray::setIndexBuffer(Rc<IndexBuffer> buf)
	{
		glBindVertexArray(vaoId);
		buf->bind();

		indexBuf = buf;
	}
}


