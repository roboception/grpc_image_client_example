/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2021 Roboception GmbH
 * All rights reserved
 *
 * Author: Felix Ruess
 */

#ifndef IMAGE_UTILS_H_
#define IMAGE_UTILS_H_

#include <string>

#include "image_interface.grpc.pb.h"

namespace utils
{
enum ImgFmt
{
  PNG
};

/**
  This method checks if the given file name already exists and produces a new
  file name if this happens.
*/

std::string ensureNewFileName(std::string name);

/**
Stores the given image.

NOTE: An exception that is based on std::exception is thrown in case of an
error, e.g. if input or output format is not supported.

@param prefix  Name of output file without suffix.
@param fmt     Image file format.
@param image   Image to be stored.
*/
std::string storeImage(const std::string& prefix, ImgFmt fmt, const Image& image);

void storeImageSet(const std::string& path, ImgFmt fmt, const ImageSet& image_set);

}  // namespace utils

#endif  // IMAGE_UTILS_H_
