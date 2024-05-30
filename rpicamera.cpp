#include "rpicamera.h"

#include <thread>
#include <chrono>

namespace rpic {

rpiCamera::rpiCamera() {
  
  setLogLevel(libcamera::LoggingTargetNone);
  cm = std::make_unique<libcamera::CameraManager>();
  if (cm->start()){
      std::cout << "Failed to start camera manager: " << std::endl;
      return;
  }
  camera = cm->cameras()[0];
  if (!camera) {
      std::cerr << "Camera not found" << std::endl;
      return;
  }

  if (camera->acquire()) {
      std::cerr << "Failed to acquire camera " << std::endl;
      return;
  }
  
  config = camera->generateConfiguration({ libcamera::StreamRole::VideoRecording });
  
}

rpiCamera::~rpiCamera() {
  camera->stop(); 
  allocator->free(config->at(0).stream());
  delete allocator;
  camera->release();
  camera.reset();
  cm.reset();
}

void rpiCamera::setLogLevel(libcamera::LoggingTarget level) {
  libcamera::logSetTarget(level);
}

void rpiCamera::open() {
  setBuffer();
  setRequest();
}

void rpiCamera::setWidth(const int width) {
  libcamera::StreamConfiguration &streamConfig = config->at(0);
  streamConfig.size.width = width;
  config->validate();
  camera->configure(config.get());
}

void rpiCamera::setHeight(const int height) {
  libcamera::StreamConfiguration &streamConfig = config->at(0);
  streamConfig.size.height = height;
  config->validate();
  camera->configure(config.get());
}

void rpiCamera::setFormat(const libcamera::PixelFormat f) {
  libcamera::StreamConfiguration &streamConfig = config->at(0);
  streamConfig.pixelFormat = f;
  config->validate();
  camera->configure(config.get());
}

void rpiCamera::setRotation(const libcamera::Orientation o) {
  config->orientation = o;
  config->validate();
  camera->configure(config.get());
}

void rpiCamera::setBuffer() {
  libcamera::StreamConfiguration &streamConfig = config->at(0);
  streamConfig.bufferCount = 1;
  config->validate();
  camera->configure(config.get());

  allocator = new libcamera::FrameBufferAllocator(camera);
  int status = allocator->allocate(config->at(0).stream());

  if (status < 0) {
      std::cerr << "Cannot allocate buffer" << std::endl;
      return;
    }
}

void rpiCamera::setRequest() {
  request = camera->createRequest();
  if (!request) {
    std::cout << "Cannot create request" << std::endl;
    return;
  }
  const std::unique_ptr<libcamera::FrameBuffer> &buffer = allocator->buffers(config->at(0).stream())[0];
  int status = request->addBuffer(config->at(0).stream(), buffer.get());
  if (status < 0) {
    std::cout << "Cannot set buffer for request" << std::endl;
    return;
  }
  camera->start();
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void rpiCamera::getSize(int &height, int &width, int &stride) {
  const libcamera::StreamConfiguration &streamConfig = config->at(0);
  width = streamConfig.size.width;
  height = streamConfig.size.height;
  stride = streamConfig.stride;
}

void rpiCamera::grab() {
  request->reuse(libcamera::Request::ReuseBuffers);
  camera->queueRequest(request.get());
}

uint8_t * rpiCamera::retrieve() {
  if (request->status() == libcamera::Request::RequestCancelled) {
    std::cout << "cannot retrieve image" << std::endl;
    return nullptr;
  }

  while (request->status() != libcamera::Request::RequestComplete) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  auto plane = request->buffers().begin()->second->planes()[0];
  return static_cast<uint8_t *> (mmap(NULL, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), plane.offset));
}

std::string  rpiCamera::printConfig() {
  return config->at(0).toString();
}
};