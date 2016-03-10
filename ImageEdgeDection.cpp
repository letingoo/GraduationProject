#include "ImageEdgeDection.h"
#include <vtkImageGradient.h>
#include <vtkImageMagnitude.h>
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkImageMapper3D.h>
#include <vtkImageLuminance.h>


ImageEdgeDection::ImageEdgeDection()
{

}


ImageEdgeDection::~ImageEdgeDection()
{
	m_imageReader = NULL;
	originalRenderer = NULL;
	gradRenderer = NULL;
	imageData = NULL;

	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 0.0;
}

void ImageEdgeDection::setFilePath(const char* filePath)
{

	int type = ImageTypeDection::detectImageType(filePath);

	if (ImageTypeDection::BMP == type)
		m_imageReader = vtkSmartPointer<vtkBMPReader>::New();
	else if (ImageTypeDection::JPEG == type)
		m_imageReader = vtkSmartPointer<vtkJPEGReader>::New();
	else if (ImageTypeDection::PNG == type)
		m_imageReader = vtkSmartPointer<vtkPNGReader>::New();
	else if (ImageTypeDection::PNM == type)
		m_imageReader = vtkSmartPointer<vtkPNMReader>::New();
	else if (ImageTypeDection::TIF == type)
		m_imageReader = vtkSmartPointer<vtkTIFFReader>::New();
	else
		return;

	//m_jpegReader = vtkJPEGReader::New();
	m_imageReader->SetFileName(filePath);
	m_imageReader->Update();
}


/*
	使用的是Roberts算法，暂时只能检测彩色图像
*/
void ImageEdgeDection::edgeDetection()
{

	if (!m_imageReader)
		return;

	// 先将彩色图像变成灰色图像。Roberts算法无法直接应用于彩色图像
	vtkSmartPointer<vtkImageLuminance> luminanceFilter =
		vtkSmartPointer<vtkImageLuminance>::New();
	luminanceFilter->SetInputConnection(m_imageReader->GetOutputPort()); 
	luminanceFilter->Update();

	vtkSmartPointer<vtkImageGradient> gradientFilter =
		vtkSmartPointer<vtkImageGradient>::New();
	gradientFilter->SetInputConnection(luminanceFilter->GetOutputPort());
	gradientFilter->SetDimensionality(2);

	vtkSmartPointer<vtkImageMagnitude> magnitudeFilter =
		vtkSmartPointer<vtkImageMagnitude>::New();
	magnitudeFilter->SetInputConnection(gradientFilter->GetOutputPort());
	magnitudeFilter->Update();

	double range[2];
	magnitudeFilter->GetOutput()->GetScalarRange(range);

	vtkSmartPointer<vtkImageShiftScale> shiftScale =
		vtkSmartPointer<vtkImageShiftScale>::New();
	shiftScale->SetOutputScalarTypeToUnsignedChar();
	shiftScale->SetScale(100 / range[1]);
	shiftScale->SetInputConnection(magnitudeFilter->GetOutputPort());
	shiftScale->Update();


	imageData = shiftScale;


	vtkSmartPointer<vtkImageActor> originalActor =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor->SetInputData(m_imageReader->GetOutput());

	vtkSmartPointer<vtkImageActor> gradActor =
		vtkSmartPointer<vtkImageActor>::New();
	gradActor->SetInputData(shiftScale->GetOutput());


	double originalViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
	double gradViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

	originalRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer->SetViewport(originalViewport);
	originalRenderer->AddActor(originalActor);
	originalRenderer->ResetCamera();
	originalRenderer->SetBackground(color);

	gradRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	gradRenderer->SetViewport(gradViewport);
	gradRenderer->AddActor(gradActor);
	gradRenderer->ResetCamera();
	gradRenderer->SetBackground(color);

	//controller->setFinished(true);
	finished = true;
}


void ImageEdgeDection::bindActorToMFCWindow(vtkMFCWindow* mfcWindow)
{

	mfcWindow->GetRenderWindow()->AddRenderer(originalRenderer);
	mfcWindow->GetRenderWindow()->AddRenderer(gradRenderer);
}
