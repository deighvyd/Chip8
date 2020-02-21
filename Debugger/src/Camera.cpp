#include "pch.h"

#include "Camera.h"

Camera::Camera()
{
	_positionX = 0.0f;
	_positionY = 0.0f;
	_positionZ = 0.0f;

	_rotationX = 0.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	_positionX = x;
	_positionY = y;
	_positionZ = z;
}


void Camera::SetRotation(float x, float y, float z)
{
	_rotationX = x;
	_rotationY = y;
	_rotationZ = z;
}

void Camera::Render()
{
	// build the camera matrix
	Vector up = { 0.0f, 1.0f, 0.0f };
	Vector lookAt = { 0.0f, 0.0f, 1.0f };
	Vector position = { _positionX, _positionX, _positionZ };

	float pitch = _rotationX * 0.0174532925f;	// radians
	float yaw = _rotationY * 0.0174532925f;	// radians
	float roll = _rotationZ * 0.0174532925f;	// radians

	float rotationMatrix[9];
	MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

	TransformCoord(lookAt, rotationMatrix);
	TransformCoord(up, rotationMatrix);
	
	lookAt.x = position.x + lookAt.x;
	lookAt.y = position.y + lookAt.y;
	lookAt.z = position.z + lookAt.z;

	BuildViewMatrix(position, lookAt, up);
}

void Camera::MatrixRotationYawPitchRoll(float *matrix, float yaw, float pitch, float roll)
{
	// get the cosine and sin of the yaw, pitch, and roll.
	float cYaw = cosf(yaw);
	float cPitch = cosf(pitch);
	float cRoll = cosf(roll);

	float sYaw = sinf(yaw);
	float sPitch = sinf(pitch);
	float sRoll = sinf(roll);

	// calculate the yaw, pitch, roll rotation matrix.
	matrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
	matrix[1] = (sRoll * cPitch);
	matrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);
	
	matrix[3] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
	matrix[4] = (cRoll * cPitch);
	matrix[5] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);
	
	matrix[6] = (cPitch * sYaw);
	matrix[7] = -sPitch;
	matrix[8] = (cPitch * cYaw);
}

void Camera::TransformCoord(Vector& vector, float *matrix)
{
	// transform the vector by the 3x3 matrix.
	vector.x = (vector.x * matrix[0]) + (vector.y * matrix[3]) + (vector.z * matrix[6]);
	vector.y = (vector.x * matrix[1]) + (vector.y * matrix[4]) + (vector.z * matrix[7]);
	vector.z = (vector.x * matrix[2]) + (vector.y * matrix[5]) + (vector.z * matrix[8]);
}

void Camera::BuildViewMatrix(Vector position, Vector lookAt, Vector up)
{
	/*Vector zAxis, xAxis, yAxis;
	float length, result1, result2, result3;*/

	float length;

	// zAxis = normal(lookAt - position)
	Vector zAxis;
	zAxis.x = lookAt.x - position.x;
	zAxis.y = lookAt.y - position.y;
	zAxis.z = lookAt.z - position.z;
	length = sqrt((zAxis.x * zAxis.x) + (zAxis.y * zAxis.y) + (zAxis.z * zAxis.z));
	zAxis.x = zAxis.x / length;
	zAxis.y = zAxis.y / length;
	zAxis.z = zAxis.z / length;

	// xAxis = normal(cross(up, zAxis))
	Vector xAxis;
	xAxis.x = (up.y * zAxis.z) - (up.z * zAxis.y);
	xAxis.y = (up.z * zAxis.x) - (up.x * zAxis.z);
	xAxis.z = (up.x * zAxis.y) - (up.y * zAxis.x);
	length = sqrt((xAxis.x * xAxis.x) + (xAxis.y * xAxis.y) + (xAxis.z * xAxis.z));
	xAxis.x = xAxis.x / length;
	xAxis.y = xAxis.y / length;
	xAxis.z = xAxis.z / length;

	// yAxis = cross(zAxis, xAxis)
	Vector yAxis;
	yAxis.x = (zAxis.y * xAxis.z) - (zAxis.z * xAxis.y);
	yAxis.y = (zAxis.z * xAxis.x) - (zAxis.x * xAxis.z);
	yAxis.z = (zAxis.x * xAxis.y) - (zAxis.y * xAxis.x);

	// -dot(xAxis, position)
	float result1 = ((xAxis.x * position.x) + (xAxis.y * position.y) + (xAxis.z * position.z)) * -1.0f;

	// -dot(yaxis, eye)
	float result2 = ((yAxis.x * position.x) + (yAxis.y * position.y) + (yAxis.z * position.z)) * -1.0f;

	// -dot(zaxis, eye)
	float result3 = ((zAxis.x * position.x) + (zAxis.y * position.y) + (zAxis.z * position.z)) * -1.0f;

	// set the matrix
	_viewMatrix[0]  = xAxis.x;
	_viewMatrix[1]  = yAxis.x;
	_viewMatrix[2]  = zAxis.x;
	_viewMatrix[3]  = 0.0f;

	_viewMatrix[4]  = xAxis.y;
	_viewMatrix[5]  = yAxis.y;
	_viewMatrix[6]  = zAxis.y;
	_viewMatrix[7]  = 0.0f;

	_viewMatrix[8]  = xAxis.z;
	_viewMatrix[9]  = yAxis.z;
	_viewMatrix[10] = zAxis.z;
	_viewMatrix[11] = 0.0f;

	_viewMatrix[12] = result1;
	_viewMatrix[13] = result2;
	_viewMatrix[14] = result3;
	_viewMatrix[15] = 1.0f;
}

void Camera::GetViewMatrix(float *matrix)
{
	matrix[0]  = _viewMatrix[0];
	matrix[1]  = _viewMatrix[1];
	matrix[2]  = _viewMatrix[2];
	matrix[3]  = _viewMatrix[3];

	matrix[4]  = _viewMatrix[4];
	matrix[5]  = _viewMatrix[5];
	matrix[6]  = _viewMatrix[6];
	matrix[7]  = _viewMatrix[7];

	matrix[8]  = _viewMatrix[8];
	matrix[9]  = _viewMatrix[9];
	matrix[10] = _viewMatrix[10];
	matrix[11] = _viewMatrix[11];

	matrix[12] = _viewMatrix[12];
	matrix[13] = _viewMatrix[13];
	matrix[14] = _viewMatrix[14];
	matrix[15] = _viewMatrix[15];
}
