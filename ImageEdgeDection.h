pragma once

#include <vtkMFCWindow.h>
#include <vtkImageReader2.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkBMPReader.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkTIFFReader.h>
#include <vtkPNMReader.h>
#include <vtkAlgorithmOutput.h>
#include <vtkImageShiftScale.h>
#include "ImageTypeDection.h"
#include "IAlgorithm.h"

//#include "LController.h"
/*
	图像边缘检测的核心类，但其具体算法并不包含在内，而是委托给IAlgorithm类
*/
class ImageEdgeDection
{
public:
	ImageEdgeDection();
	~ImageEdgeDection();
	//ImageEdgeDection(LController* m_controller);

	void setFilePath(const char* filePath);		// 设置需要检测的图片文件路径
	void edgeDetection();			// 进行图片边缘检测
	void bindActorToMFCWindow(vtkMFCWindow* mfcWindow);
	void setFinished(bool finished){ this->finished = finished; }
	bool getFinished(){ return finished; }
	vtkSmartPointer<vtkImageShiftScale> getImageDataOutput(){ return imageData; }	// 得到处理后的图片信息


private:
	vtkSmartPointer<vtkImageReader2> m_imageReader;
	vtkSmartPointer<vtkRenderer> originalRenderer;
	vtkSmartPointer<vtkRenderer> gradRenderer;

	vtkSmartPointer<vtkImageShiftScale> imageData;

	bool finished = false;		// 记录是否完成图像边缘检测

	IAlgorightm* algorithm = NULL;		// 图像边缘检测的算法
	double color[3];		// VTKRenderer的背景颜色
};
