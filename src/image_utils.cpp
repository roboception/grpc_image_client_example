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

#include "image_utils.h"

#include <string>
#include <exception>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

#ifdef INCLUDE_PNG
#include <png.h>
#endif

namespace utils
{
std::string ensureNewFileName(std::string name)
{
  // check if given name is already used

  std::ifstream file(name);

  if (file.is_open())
  {
    file.close();

    // split name in prefix and suffix

    std::string suffix;

    size_t i = name.rfind('.');
    if (i != name.npos && name.size() - i <= 4)
    {
      suffix = name.substr(i);
      name = name.substr(0, i);
    }

    // add number for finding name that is nor used

    int n = 1;
    while (n < 100)
    {
      std::ostringstream s;
      s << name << "_" << n << suffix;

      file.open(s.str());
      if (!file.is_open())
      {
        name = s.str();
        break;
      }

      file.close();
      n++;
    }
  }

  return name;
}

namespace
{
class IOException : public std::exception
{
public:
  IOException(const std::string& _msg)
  {
    msg = _msg;
  }
  virtual const char* what() const noexcept
  {
    return msg.c_str();
  }

private:
  std::string msg;
};

#ifdef INCLUDE_PNG

std::string storeImagePNG(const std::string& name, const Image& image)
{
  size_t width = image.width();
  size_t height = image.height();

  const unsigned char* p = reinterpret_cast<const unsigned char*>(image.data().data());

  std::string full_name;

  if (image.encoding() == "mono8")
  {
    // open file and init

    full_name = ensureNewFileName(name + ".png");
    FILE* out = fopen(full_name.c_str(), "wb");

    if (!out)
    {
      throw new IOException("Cannot store file: " + full_name);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop info = png_create_info_struct(png);
    setjmp(png_jmpbuf(png));

    // write header

    png_init_io(png, out);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // write image body

    for (size_t k = 0; k < height; k++)
    {
      png_write_row(png, const_cast<png_bytep>(p));
      p += image.step();
    }

    // close file

    png_write_end(png, info);
    fclose(out);
    png_destroy_write_struct(&png, &info);
  }
  else if (image.encoding() == "mono16")
  {
    // store 16 bit monochrome image
    // open file and init

    full_name = ensureNewFileName(name + ".png");
    FILE* out = fopen(full_name.c_str(), "wb");

    if (!out)
    {
      throw new IOException("Cannot store file: " + full_name);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop info = png_create_info_struct(png);
    setjmp(png_jmpbuf(png));

    // write header

    png_init_io(png, out);
    png_set_IHDR(png, info, width, height, 16, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // write image body

    if (!image.is_bigendian())
    {
      png_set_swap(png);
    }

    for (size_t k = 0; k < height; k++)
    {
      png_write_row(png, const_cast<png_bytep>(p));
      p += image.step();
    }

    // close file

    png_write_end(png, info);
    fclose(out);
    png_destroy_write_struct(&png, &info);
  }

  return full_name;
}

#endif

}  // namespace

std::string storeImage(const std::string& name, ImgFmt fmt, const Image& image)
{
  std::string ret;

  switch (fmt)
  {
    case PNG:
#ifdef INCLUDE_PNG
      ret = storeImagePNG(name, image);
#else
      throw IOException("storeImage(): Support for PNG image file format is not compiled in!");
#endif
      break;

    default:
      throw IOException("storeImage(): No supported image file format is compiled in!");
      break;
  }

  return ret;
}

void storeImageSet(const std::string& path, ImgFmt fmt, const ImageSet& image_set)
{
  std::ostringstream ts_str;
  ts_str << image_set.timestamp().sec() << "." << std::setfill('0') << std::setw(9) << image_set.timestamp().nsec();
  std::string prefix = path + "image_" + ts_str.str() + "_";
  if (image_set.has_left())
  {
    storeImage(prefix + "left", PNG, image_set.left());
  }
  if (image_set.has_right())
  {
    storeImage(prefix + "right", PNG, image_set.right());
  }
  // TODO: store metadata
  if (image_set.has_disparity())
  {
    storeImage("disparity", PNG, image_set.disparity().image());
  }
  if (image_set.has_confidence())
  {
    storeImage(prefix + "confidence", PNG, image_set.confidence());
  }
  if (image_set.has_disparity_error())
  {
    storeImage(prefix + "error", PNG, image_set.disparity_error());
  }
}

}  // namespace utils
