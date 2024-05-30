#ifndef RPICAMERA_H
#define RPICAMERA_H

#include <sys/mman.h>
#include <libcamera/libcamera.h>

namespace rpic {

class rpiCamera {
    public:
        rpiCamera();
        ~rpiCamera();
        void setLogLevel(libcamera::LoggingTarget target);       
        void open();
        void setWidth(const int width);
        void setHeight(const int height);
        void setFormat(const libcamera::PixelFormat format);
        void setRotation(const libcamera::Orientation o);
        void setBuffer();
        void setRequest();
        void getSize(int &height, int &width, int &stride);
        uint8_t* retrieve();
        void grab();

        std::unique_ptr<libcamera::CameraManager> cm;
        
        std::string printConfig();

        libcamera::FrameBufferAllocator *allocator;
        std::shared_ptr<libcamera::Camera> camera;
        std::unique_ptr<libcamera::CameraConfiguration> config;
        std::unique_ptr<libcamera::Request> request; 
};
};

#endif