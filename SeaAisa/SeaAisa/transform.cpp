#include"transform.h"


//Matrix4x4 operators
Matrix4x4::Matrix4x4(float mat[4][4]) {
	memcpy(m, mat, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(float t00, float t01, float t02, float t03,
	float t10, float t11, float t12, float t13,
	float t20, float t21, float t22, float t23,
	float t30, float t31, float t32, float t33) {
	m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
	m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
	m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
	m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
}

Matrix4x4 Transpose(const Matrix4x4 &m) {
	return Matrix4x4(m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
}


Matrix4x4 Inverse(const Matrix4x4 &m) {
	return Matrix4x4(m);
}

//Transform operator
Transform Translate(const Vector &delta) {
	Matrix4x4 m(1, 0, 0, delta.x,
		0, 1, 0, delta.y,
		0, 0, 1, delta.z,
		0, 0, 0, 1);
	Matrix4x4 minv(1, 0, 0, -delta.x,
		0, 1, 0, -delta.y,
		0, 0, 1, -delta.z,
		0, 0, 0, 1);
	return Transform(m, minv);
}


Transform Scale(float x, float y, float z) {
	Matrix4x4 m(x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1);
	Matrix4x4 minv(1.f / x, 0, 0, 0,
		0, 1.f / y, 0, 0,
		0, 0, 1.f / z, 0,
		0, 0, 0, 1);
	return Transform(m, minv);
}

Transform RotateX(float angle) {
	float sin_t = sinf(Radians(angle));
	float cos_t = cosf(Radians(angle));
	Matrix4x4 m(1, 0, 0, 0,
		0, cos_t, -sin_t, 0,
		0, sin_t, cos_t, 0,
		0, 0, 0, 1);
	return Transform(m, Transpose(m));
}

Transform RotateY(float angle) {
	float sin_t = sinf(Radians(angle));
	float cos_t = cosf(Radians(angle));
	Matrix4x4 m(cos_t, 0, sin_t, 0,
		0, 1, 0, 0,
		-sin_t, 0, cos_t, 0,
		0, 0, 0, 1);
	return Transform(m, Transpose(m));
}

Transform RotateZ(float angle) {
	float sin_t = sinf(Radians(angle));
	float cos_t = cosf(Radians(angle));
	Matrix4x4 m(cos_t, -sin_t, 0, 0,
		sin_t, cos_t, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	return Transform(m, Transpose(m));
}

Transform Rotate(float angle, const Vector &axis) {
	Vector a = Normalize(axis);
	float s = sinf(Radians(angle));
	float c = cosf(Radians(angle));
	float m[4][4];

	m[0][0] = a.x * a.x + (1.f - a.x * a.x) * c;
	m[0][1] = a.x * a.y * (1.f - c) - a.z * s;
	m[0][2] = a.x * a.z * (1.f - c) + a.y * s;
	m[0][3] = 0;

	m[1][0] = a.x * a.y * (1.f - c) + a.z * s;
	m[1][1] = a.y * a.y + (1.f - a.y * a.y) * c;
	m[1][2] = a.y * a.z * (1.f - c) - a.x * s;
	m[1][3] = 0;

	m[2][0] = a.x * a.z * (1.f - c) - a.y * s;
	m[2][1] = a.y * a.z * (1.f - c) + a.x * s;
	m[2][2] = a.z * a.z + (1.f - a.z * a.z) * c;
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;

	Matrix4x4 mat(m);
	return Transform(mat, Transpose(mat));
}

Transform LookAt(const Point &eye, const Point &gaze, const Vector &up) {
	Vector xaxis, yaxis, zaxis;
	Matrix4x4 m;

	zaxis = gaze - eye;
	zaxis = Normalize(zaxis);

	xaxis = Cross(zaxis, up);
	if (Dot(zaxis, up) <= 0)
		xaxis = -Normalize(xaxis);
	else
		xaxis = Normalize(xaxis);

	if (Dot(xaxis, zaxis) <= 0)
		yaxis = -Cross(xaxis, zaxis);
	else
		yaxis = Cross(xaxis, zaxis);
	zaxis = -zaxis;

	m.m[0][0] = xaxis.x;
	m.m[0][1] = xaxis.y;
	m.m[0][2] = xaxis.z;

	m.m[1][0] = yaxis.x;
	m.m[1][1] = yaxis.y;
	m.m[1][2] = yaxis.z;

	m.m[2][0] = zaxis.x;
	m.m[2][1] = zaxis.y;
	m.m[2][2] = zaxis.z;

	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;

	m.m[0][3] = -eye.x*xaxis.x - eye.y*xaxis.y - eye.z*xaxis.z;
	m.m[1][3] = -eye.x*yaxis.x - eye.y*yaxis.y - eye.z*yaxis.z;
	m.m[2][3] = -eye.x*zaxis.x - eye.y*zaxis.y - eye.z*zaxis.z;

	return Transform(m, Inverse(m));
}
