#pragma once

class Camera
{
private:
	struct Vector
	{ 
		float x, y, z;
	};

public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void Render();
	void GetViewMatrix(float *matrix);

private:
	void MatrixRotationYawPitchRoll(float *matrix, float yaw, float pitch, float roll);
	void TransformCoord(Vector& vector, float *matrix);
	void BuildViewMatrix(Vector position, Vector lookAt, Vector up);

private:
	float _positionX;
	float _positionY;
	float _positionZ;

	float _rotationX;
	float _rotationY;
	float _rotationZ;

	float _viewMatrix[16];

};
