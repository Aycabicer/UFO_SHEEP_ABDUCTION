#include"ColoredMesh.h"



ColoredMesh::ColoredMesh(OBJFile const& objFile)
: vaoHandle{}	//set to 0
, vertexCount{static_cast<GLsizei>(objFile.positions.size())}
{
	positions = std::move(VBO{objFile.positions});
	colors = std::move(VBO{objFile.colors});
	normals = std::move(VBO{objFile.normals});

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glBindBuffer(GL_ARRAY_BUFFER, positions.handle);
	glVertexAttribPointer(
		0, // location = 0 in vertex shader 
		3, GL_FLOAT, GL_FALSE, // 3 floats, not normalized to [0..1] (GL FALSE) 
		0, // stride = 0 indicates that there is no padding between inputs 
		0 // data starts at offset 0 in the VBO.
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colors.handle);
	glVertexAttribPointer(
		1, // location = 1 in vertex shader 
		3, GL_FLOAT, GL_FALSE, // 3 floats, not normalized to [0..1] (GL FALSE) 
		0, // stride = 0 indicates that there is no padding between inputs 
		0 // data starts at offset 0 in the VBO.
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normals.handle);
	glVertexAttribPointer(
		2, // location = 2 in vertex shader 
		3, GL_FLOAT, GL_FALSE, // 3 floats, not normalized to [0..1] (GL FALSE) 
		0, // see above 
		0 // see above 
	);
	glEnableVertexAttribArray(2);
}
