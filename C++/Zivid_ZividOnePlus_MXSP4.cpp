//***************************************************************************************/
// 
// File name: Zivid_ZividOnePlus_MXSP4.cpp  
//
// Synopsis:  This program contains an example of 3d reconstruction by interfacing with
//            a ZividLabs Zivid scanner.
//            See the PrintHeader() function below for detailed description.
//
// Copyright © 1992-2024 Zebra Technologies Corp. and/or its affiliates
// All Rights Reserved
//***************************************************************************************/

#include <mil.h>
#include <iostream>
#include "MainTemplate.h"

// Once the Zivid SDK is installed and the project is configured correctly (see
// PrintHeader()), set ZIVID_SDK_INSTALLED to 1 to enable the Zivid-specific code.
// You need an actual Zivid connected to your computer.
#define ZIVID_SDK_INSTALLED  0

//****************************************************************************
// Example description.
//****************************************************************************
void PrintHeader()
{
   MosPrintf(MIL_TEXT("[EXAMPLE NAME]\n")
      MIL_TEXT("Zivid_ZividOnePlus_MXSP4\n\n")

      MIL_TEXT("[SYNOPSIS]\n")
      MIL_TEXT("This program acquires a 3d point cloud using a ZividLabs Zivid sensor\n")
      MIL_TEXT("with the Zivid API. It then converts the point cloud to the MIL\n")
      MIL_TEXT("format and displays the result.\n\n")

      MIL_TEXT("[MODULES USED]\n")
      MIL_TEXT("Modules used: application, system, buffer, 3D Display,\n")
      MIL_TEXT("              3D Graphics, 3D Processing.\n\n"));
}

//*****************************************************************************
// ZIVID-specific header, constants
//*****************************************************************************

#if ZIVID_SDK_INSTALLED

#if defined(WIN32)
#error "Zivid api cannot be used in a 32-bit application."
#endif

#if _DEBUG
#pragma comment(lib, "lib_debug\\ZividAPId.lib")
#else
#pragma comment(lib, "lib\\ZividAPI.lib")
#endif

// The project options must be modified so that this header is reachable.
#include <Zivid\Zivid.h>

// Timeout related to acquisition time.
static const double RECEIVE_TIMEOUT = 5000;  // in ms

struct SZividObjects
{
   Zivid::Application Application;
   Zivid::Camera      Camera;
};


//*****************************************************************************
// Helper function prototypes
//*****************************************************************************

// 3d camera interface functions.
template <> bool Init(SZividObjects* pSensor3d);
template <> bool SetupForExample(SZividObjects* pSensor3d);
template <> bool Grab(SZividObjects* pSensor3d, MIL_ID MilContainer);
template <> bool Free(SZividObjects* pSensor3d);

// Function for example code.
template <class T>
MIL_ID GetZividComponent(MIL_ID MilSystem, MIL_ID MilContainer, MIL_INT ComponentIdFlag,
   MIL_INT NbBands, MIL_INT SizeX, MIL_INT SizeY, MIL_INT Type, MIL_INT Attribute,
   T* pData, MIL_INT* pPitch);

#endif

//*****************************************************************************
// Main.
//*****************************************************************************
int MosMain()
{
   PrintHeader();

#if !ZIVID_SDK_INSTALLED
   MosPrintf(MIL_TEXT("This example is designed to be used with a Zivid sensor and\n"));
   MosPrintf(MIL_TEXT("the Zivid API. To run the example:\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Install the Zivid SDK.\n"));
   MosPrintf(MIL_TEXT("\n"));

   MosPrintf(MIL_TEXT("- An environment variable named Zivid_install_folder should exist, with its value corresponding\n"));
   MosPrintf(MIL_TEXT("  to the path of the Zivid_install_folder folder.\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Copy the shared libraries to your system path. Under Windows, copy\n"));
   MosPrintf(MIL_TEXT("    $(Zivid_install_folder)\\bin\\ZividAPI.dll\n"));
   MosPrintf(MIL_TEXT("  to\n"));
   MosPrintf(MIL_TEXT("    %%SYSTEMROOT%%\\System32\\   (usually C:\\Windows\\System32\\)\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Add the paths to the header files and library files of the Zivid SDK to\n"));
   MosPrintf(MIL_TEXT("  the example project files. If you are using Visual Studio, open the Property\n"));
   MosPrintf(MIL_TEXT("  Pages of the Zivid_ZividOnePlus_MXSP4 project. Then, under Configuration Properties,\n"));
   MosPrintf(MIL_TEXT("  you must:\n"));
   MosPrintf(MIL_TEXT("  - Add\n"));
   MosPrintf(MIL_TEXT("      $(Zivid_install_folder)\\include\n"));
   MosPrintf(MIL_TEXT("    to\n"));
   MosPrintf(MIL_TEXT("      C/C++->General->Additional Include Directories\n"));
   MosPrintf(MIL_TEXT("  - Add \n"));
   MosPrintf(MIL_TEXT("      $(Zivid_install_folder) \n"));
   MosPrintf(MIL_TEXT("    to \n"));
   MosPrintf(MIL_TEXT("      Linker->General->Additional Library Directories\n"));
   MosPrintf(MIL_TEXT("\n"));

   MosPrintf(MIL_TEXT("- Update the example code:\n"));
   MosPrintf(MIL_TEXT("  - Set the ZIVID_SDK_INSTALLED define to 1.\n"));
   MosPrintf(MIL_TEXT("  - Modify the SetupForExample function if some settings need to be changed.\n"));
   MosPrintf(MIL_TEXT("  - Recompile the example.\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("The example has been tested with the following setups:\n"));
   MosPrintf(MIL_TEXT("- Windows 10 64-bit and VS2017.\n"));
   MosPrintf(MIL_TEXT("- MIL 10 Sp4 Build 602 (EA).\n"));
   MosPrintf(MIL_TEXT("- zivid SDK 1.5, Zivid One+ Small camera.\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("Press <Enter> to end.\n"));
   MosGetch();
#else

   MosPrintf(MIL_TEXT("Press <Enter> to start.\n\n"));
   MosGetch();

   SZividObjects Sensor3d;
   Interface3dCameraExample(&Sensor3d, NAN);

#endif
   return 0;
}

#if ZIVID_SDK_INSTALLED

//*****************************************************************************
// Connects the Zivid.
//*****************************************************************************
template <> bool Init(SZividObjects* p3dSensor)
{
   try
   {
      // Connect the camera.
      MosPrintf(MIL_TEXT("Zivid: Connecting the camera... "));
      p3dSensor->Camera = p3dSensor->Application.connectCamera();
      MosPrintf(MIL_TEXT("OK.\n"));
      return true;
   }
   catch (const std::exception& e)
   {
      MosPrintfA("\nZivid: Exception: %s.\n", e.what());
      return false;
   }
}

//*****************************************************************************
// Setup the Zivid for the example.
//*****************************************************************************
static const std::chrono::microseconds EXPOSURE_TIME(8333); // in us
static const MIL_DOUBLE                OUTLIER_THRESHOLD = 5;
static const size_t                    CAMERA_IRIS = 22;
template <> bool SetupForExample(SZividObjects* p3dSensor)
{
   try
   {
      // Setup the acquisition.
      p3dSensor->Camera << Zivid::Settings::Iris{ CAMERA_IRIS }
         << Zivid::Settings::ExposureTime{ EXPOSURE_TIME }
         << Zivid::Settings::Filters::Outlier::Enabled::yes
         << Zivid::Settings::Filters::Outlier::Threshold{ OUTLIER_THRESHOLD };
      return true;
   }
   catch (const std::exception& e)
   {
      MosPrintfA("\nZivid: Exception: %s.\n", e.what());
      return false;
   }
}

//*****************************************************************************
// Frees the device.
//*****************************************************************************
template <> bool Free(SZividObjects* p3dSensor)
{
   try
   {
      // Disconnect the camera.
      p3dSensor->Camera.disconnect();

      return true;
   }
   catch (const std::exception& e)
   {
      MosPrintfA("\nZivid: Exception: %s.\n", e.what());
      return false;
   }
}

//*****************************************************************************
// Triggers and acquires the data and put it in a MIL container.
//*****************************************************************************
template <> bool Grab(SZividObjects* p3dSensor, MIL_ID MilContainer)
{
   try
   {
      // Capture a frame.
      MosPrintf(MIL_TEXT("Zivid: Capturing a frame... "));
      Zivid::Frame Frame = p3dSensor->Camera.capture();
      MosPrintf(MIL_TEXT("OK.\n"));

      MosPrintf(MIL_TEXT("Zivid: Acquiring 3d point cloud data... "));
      Zivid::PointCloud PointCloud = Frame.getPointCloud();
      MosPrintf(MIL_TEXT("OK.\n"));

      // Add the component buffers if necessary.
      MIL_ID MilSystem = MbufInquire(MilContainer, M_OWNER_SYSTEM, M_NULL);

      // Add the point cloud component.
      SPoint3d* pPointCloud = nullptr;
      MIL_INT PointCloudPitch;
      MIL_ID RangeComponent = GetZividComponent(MilSystem, MilContainer, M_COMPONENT_RANGE,
                              3, PointCloud.width(), PointCloud.height(), 32 + M_FLOAT, M_IMAGE + M_PROC + M_PACKED + M_RGB96,
                              &pPointCloud, &PointCloudPitch);

      // Get the "reflectance" component.
      SColor* pReflectance = nullptr;
      MIL_INT ReflectancePitch;
      MIL_ID ReflectanceComponent = GetZividComponent(MilSystem, MilContainer, M_COMPONENT_REFLECTANCE,
                                    3, PointCloud.width(), PointCloud.height(), 8 + M_UNSIGNED, M_IMAGE + M_PROC + M_BGR32 + M_PACKED,
                                    &pReflectance, &ReflectancePitch);

      //// Get the "contrast" component into the confidence.
      MIL_FLOAT* pConfidence = nullptr;
      MIL_INT ConfidencePitch;
      MIL_ID ConfidenceComponent = GetZividComponent(MilSystem, MilContainer, M_COMPONENT_CONFIDENCE,
                                   1, PointCloud.width(), PointCloud.height(), 32 + M_FLOAT, M_IMAGE + M_PROC,
                                   &pConfidence, &ConfidencePitch);

      // Extract the components.
      size_t p = 0;
      for (size_t y = 0; y < PointCloud.height(); y++)
      {
         for (size_t x = 0; x < PointCloud.width(); x++)
         {
            Zivid::Point& rPoint = PointCloud(p);
            pPointCloud[p].x = rPoint.x;
            pPointCloud[p].y = rPoint.y;
            pPointCloud[p].z = rPoint.z;
            pReflectance[x + y * ReflectancePitch] = *((SColor*)&rPoint.rgba);
            pConfidence[x + y * ConfidencePitch] = rPoint.contrast;
            p++;
         }
      }
      return true;
   }
   catch (const std::exception& e)
   {
      MosPrintfA("\nZivid: Exception: %s.\n", e.what());
      return false;
   }
}

//*****************************************************************************
// Gets an individual component from a MIL container. If it is not present,
// Allocates a component if it is not present.
//*****************************************************************************
template <class T>
MIL_ID GetZividComponent(MIL_ID MilSystem, MIL_ID MilContainer, MIL_INT ComponentIdFlag,
                        MIL_INT NbBands, MIL_INT SizeX, MIL_INT SizeY, MIL_INT Type, MIL_INT Attribute,
                        T* pData, MIL_INT* pPitch)
{
   MIL_ID MilComponent = MbufInquireContainer(MilContainer, ComponentIdFlag, M_COMPONENT_ID, M_NULL);

   if (!MilComponent)
   {
      MbufAllocComponent(MilContainer, NbBands, SizeX, SizeY, Type, Attribute, ComponentIdFlag, &MilComponent);
   }
   MbufInquire(MilComponent, M_HOST_ADDRESS, ((void**)pData));
   MbufInquire(MilComponent, M_PITCH, pPitch);
   return MilComponent;
}

#endif
