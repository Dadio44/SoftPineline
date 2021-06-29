#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:

	glm::vec3 GetForward()const
	{
		return _lookAt - _pos;
	}

	void SetPos(glm::vec3 pos)
	{
		_pos = pos;
		UpdateView();
	}

	glm::vec3 GetPos()const
	{
		return _pos;
	}

	void SetTarget(glm::vec3 lookAt)
	{
		_lookAt = lookAt;
		UpdateView();
	}

	void SetPerspective(
		float fovy,
		float aspect,
		float zNear,
		float zFar)
	{
		_fovy = fovy;
		_aspect = aspect;
		_zNear = zNear;
		_zFar = zFar;

		_projection = glm::perspective(_fovy, _aspect,_zNear,_zFar);

		//UpdateViewProjection();
	}

	glm::mat4x4 GetView()const
	{
		return _view;
	}

	glm::mat4x4 GetProjection()const 
	{
		return _projection;
	}

private:
	void UpdateView()
	{
		_view = glm::lookAt(_pos, _lookAt, glm::vec3(0, 1, 0));
	}

	//void UpdateViewProjection()
	//{
	//	_viewProjection = _projection * _view;
	//}

	glm::vec3 _pos;
	glm::vec3 _lookAt;
	glm::mat4x4 _view;


	float _fovy;
	float _aspect;
	float _zNear;
	float _zFar;
	glm::mat4x4 _projection;

	//glm::mat4x4 _viewProjection;
};

