/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2014 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

// mapnik
#include <mapnik/image_util.hpp>
#include <mapnik/image_util_jpeg.hpp>
#include <mapnik/image_util_png.hpp>
#include <mapnik/image_util_tiff.hpp>
#include <mapnik/image_util_webp.hpp>
#include <mapnik/image_data.hpp>
#include <mapnik/image_data_any.hpp>
#include <mapnik/memory.hpp>
#include <mapnik/image_view.hpp>
#include <mapnik/palette.hpp>
#include <mapnik/map.hpp>
#include <mapnik/util/conversions.hpp>
#include <mapnik/util/variant.hpp>

// boost
#include <boost/tokenizer.hpp>

// agg
#include "agg_rendering_buffer.h"
#include "agg_pixfmt_rgba.h"
#include "agg_color_rgba.h"

// stl
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace mapnik
{

template <typename T>
std::string save_to_string(T const& image,
                           std::string const& type,
                           rgba_palette const& palette)
{
    std::ostringstream ss(std::ios::out|std::ios::binary);
    save_to_stream(image, ss, type, palette);
    return ss.str();
}

template <typename T>
std::string save_to_string(T const& image,
                           std::string const& type)
{
    std::ostringstream ss(std::ios::out|std::ios::binary);
    save_to_stream(image, ss, type);
    return ss.str();
}

template <typename T>
void save_to_file(T const& image,
                  std::string const& filename,
                  std::string const& type,
                  rgba_palette const& palette)
{
    std::ofstream file (filename.c_str(), std::ios::out| std::ios::trunc|std::ios::binary);
    if (file)
    {
        save_to_stream<T>(image, file, type, palette);
    }
    else throw ImageWriterException("Could not write file to " + filename );
}

template <typename T>
void save_to_file(T const& image,
                  std::string const& filename,
                  std::string const& type)
{
    std::ofstream file (filename.c_str(), std::ios::out| std::ios::trunc|std::ios::binary);
    if (file)
    {
        save_to_stream<T>(image, file, type);
    }
    else throw ImageWriterException("Could not write file to " + filename );
}

template <typename T>
void save_to_stream(T const& image,
                    std::ostream & stream,
                    std::string const& type,
                    rgba_palette const& palette)
{
    if (stream && image.width() > 0 && image.height() > 0)
    {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == "png" || boost::algorithm::starts_with(t, "png"))
        {
            png_saver_pal visitor(stream, t, palette);
            mapnik::util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "tif"))
        {
            throw ImageWriterException("palettes are not currently supported when writing to tiff format (yet)");
        }
        else if (boost::algorithm::starts_with(t, "jpeg"))
        {
            throw ImageWriterException("palettes are not currently supported when writing to jpeg format");
        }
        else throw ImageWriterException("unknown file type: " + type);
    }
    else throw ImageWriterException("Could not write to empty stream" );
}

// This can be removed once image_data_any and image_view_any are the only 
// items using this template
template <>
void save_to_stream<image_data_rgba8>(image_data_rgba8 const& image,
                    std::ostream & stream,
                    std::string const& type,
                    rgba_palette const& palette)
{
    if (stream && image.width() > 0 && image.height() > 0)
    {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == "png" || boost::algorithm::starts_with(t, "png"))
        {
            png_saver_pal visitor(stream, t, palette);
            visitor(image);
            //mapnik::util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "tif"))
        {
            throw ImageWriterException("palettes are not currently supported when writing to tiff format (yet)");
        }
        else if (boost::algorithm::starts_with(t, "jpeg"))
        {
            throw ImageWriterException("palettes are not currently supported when writing to jpeg format");
        }
        else throw ImageWriterException("unknown file type: " + type);
    }
    else throw ImageWriterException("Could not write to empty stream" );
}

// This can be removed once image_data_any and image_view_any are the only 
// items using this template
template <>
void save_to_stream<image_view_rgba8>(image_view_rgba8 const& image,
                    std::ostream & stream,
                    std::string const& type,
                    rgba_palette const& palette)
{
    if (stream && image.width() > 0 && image.height() > 0)
    {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == "png" || boost::algorithm::starts_with(t, "png"))
        {
            png_saver_pal visitor(stream, t, palette);
            visitor(image);
            //mapnik::util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "tif"))
        {
            throw ImageWriterException("palettes are not currently supported when writing to tiff format (yet)");
        }
        else if (boost::algorithm::starts_with(t, "jpeg"))
        {
            throw ImageWriterException("palettes are not currently supported when writing to jpeg format");
        }
        else throw ImageWriterException("unknown file type: " + type);
    }
    else throw ImageWriterException("Could not write to empty stream" );
}

template <typename T>
void save_to_stream(T const& image,
                    std::ostream & stream,
                    std::string const& type)
{
    if (stream && image.width() > 0 && image.height() > 0)
    {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == "png" || boost::algorithm::starts_with(t, "png"))
        {
            png_saver visitor(stream, t);
            util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "tif"))
        {
            tiff_saver visitor(stream, t);  
            util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "jpeg"))
        {
            jpeg_saver visitor(stream, t);
            util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "webp"))
        {
            webp_saver visitor(stream, t);
            util::apply_visitor(visitor, image);
        }
        else throw ImageWriterException("unknown file type: " + type);
    }
    else throw ImageWriterException("Could not write to empty stream" );
}

// This can be removed once image_data_any and image_view_any are the only 
// items using this template
template <>
void save_to_stream<image_data_rgba8>(image_data_rgba8 const& image,
                    std::ostream & stream,
                    std::string const& type)
{
    if (stream && image.width() > 0 && image.height() > 0)
    {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == "png" || boost::algorithm::starts_with(t, "png"))
        {
            png_saver visitor(stream, t);
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "tif"))
        {
            tiff_saver visitor(stream, t);  
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "jpeg"))
        {
            jpeg_saver visitor(stream, t);
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "webp"))
        {
            webp_saver visitor(stream, t);
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else throw ImageWriterException("unknown file type: " + type);
    }
    else throw ImageWriterException("Could not write to empty stream" );
}

// This can be removed once image_data_any and image_view_any are the only 
// items using this template
template <>
void save_to_stream<image_view_rgba8>(image_view_rgba8 const& image,
                    std::ostream & stream,
                    std::string const& type)
{
    if (stream && image.width() > 0 && image.height() > 0)
    {
        std::string t = type;
        std::transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == "png" || boost::algorithm::starts_with(t, "png"))
        {
            png_saver visitor(stream, t);
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "tif"))
        {
            tiff_saver visitor(stream, t);  
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "jpeg"))
        {
            jpeg_saver visitor(stream, t);
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else if (boost::algorithm::starts_with(t, "webp"))
        {
            webp_saver visitor(stream, t);
            visitor(image);
            //util::apply_visitor(visitor, image);
        }
        else throw ImageWriterException("unknown file type: " + type);
    }
    else throw ImageWriterException("Could not write to empty stream" );
}

template <typename T>
void save_to_file(T const& image, std::string const& filename)
{
    boost::optional<std::string> type = type_from_filename(filename);
    if (type)
    {
        save_to_file<T>(image, filename, *type);
    }
    else throw ImageWriterException("Could not write file to " + filename );
}

template <typename T>
void save_to_file(T const& image, std::string const& filename, rgba_palette const& palette)
{
    boost::optional<std::string> type = type_from_filename(filename);
    if (type)
    {
        save_to_file<T>(image, filename, *type, palette);
    }
    else throw ImageWriterException("Could not write file to " + filename );
}

// image_data_rgba8
template void save_to_file<image_data_rgba8>(image_data_rgba8 const&,
                                             std::string const&,
                                             std::string const&);

template void save_to_file<image_data_rgba8>(image_data_rgba8 const&,
                                             std::string const&,
                                             std::string const&,
                                             rgba_palette const& palette);

template void save_to_file<image_data_rgba8>(image_data_rgba8 const&,
                                             std::string const&);

template void save_to_file<image_data_rgba8>(image_data_rgba8 const&,
                                             std::string const&,
                                             rgba_palette const& palette);

template std::string save_to_string<image_data_rgba8>(image_data_rgba8 const&,
                                                      std::string const&);

template std::string save_to_string<image_data_rgba8>(image_data_rgba8 const&,
                                                      std::string const&,
                                                      rgba_palette const& palette);

// image_view_rgba8
template void save_to_file<image_view_rgba8> (image_view_rgba8 const&,
                                              std::string const&,
                                              std::string const&);

template void save_to_file<image_view_rgba8> (image_view_rgba8 const&,
                                              std::string const&,
                                              std::string const&,
                                              rgba_palette const& palette);

template void save_to_file<image_view_rgba8> (image_view_rgba8 const&,
                                              std::string const&);

template void save_to_file<image_view_rgba8> (image_view_rgba8 const&,
                                              std::string const&,
                                              rgba_palette const& palette);

template std::string save_to_string<image_view_rgba8> (image_view_rgba8 const&,
                                                       std::string const&);

template std::string save_to_string<image_view_rgba8> (image_view_rgba8 const&,
                                                       std::string const&,
                                                       rgba_palette const& palette);

// image_data_any
template void save_to_file<image_data_any>(image_data_any const&,
                                           std::string const&,
                                           std::string const&);

template void save_to_file<image_data_any>(image_data_any const&,
                                           std::string const&,
                                           std::string const&,
                                           rgba_palette const& palette);

template void save_to_file<image_data_any>(image_data_any const&,
                                           std::string const&);

template void save_to_file<image_data_any>(image_data_any const&,
                                           std::string const&,
                                           rgba_palette const& palette);

template std::string save_to_string<image_data_any>(image_data_any const&,
                                                    std::string const&);

template std::string save_to_string<image_data_any>(image_data_any const&,
                                                    std::string const&,
                                                    rgba_palette const& palette);


namespace detail {

struct premultiply_visitor
{
    template <typename T>
    void operator() (T & data) 
    {
        throw std::runtime_error("Error: Premultiply with " + std::string(typeid(data).name()) + " is not supported");
    }

};

template <>
void premultiply_visitor::operator()<image_data_rgba8> (image_data_rgba8 & data)
{
    if (!data.get_premultiplied())
    {
        agg::rendering_buffer buffer(data.getBytes(),data.width(),data.height(),data.width() * 4);
        agg::pixfmt_rgba32 pixf(buffer);
        pixf.premultiply();
        data.set_premultiplied(true);
    }
}

struct demultiply_visitor
{
    template <typename T>
    void operator() (T & data) 
    {
        throw std::runtime_error("Error: Premultiply with " + std::string(typeid(data).name()) + " is not supported");
    }

};

template <>
void demultiply_visitor::operator()<image_data_rgba8> (image_data_rgba8 & data)
{
    if (data.get_premultiplied())
    {
        agg::rendering_buffer buffer(data.getBytes(),data.width(),data.height(),data.width() * 4);
        agg::pixfmt_rgba32_pre pixf(buffer);
        pixf.demultiply();
        data.set_premultiplied(false);
    }
}

struct set_premultiplied_visitor
{
    set_premultiplied_visitor(bool status)
        : status_(status) {}

    template <typename T>
    void operator() (T & data) 
    {
        data.set_premultiplied(status_);
    }
  private:
    bool status_;
};

} // end detail ns

template <typename T>
MAPNIK_DECL void premultiply_alpha(T & image)
{
    util::apply_visitor(detail::premultiply_visitor(), image);
}

template void premultiply_alpha<image_data_any> (image_data_any &);

// Temporary, can be removed once image_view_any and image_data_any are the only ones passed
template <>
MAPNIK_DECL void premultiply_alpha<image_data_rgba8>(image_data_rgba8 & image)
{
    detail::premultiply_visitor visit;
    visit(image);
}

template <typename T>
MAPNIK_DECL void demultiply_alpha(T & image)
{
    util::apply_visitor(detail::demultiply_visitor(), image);
}

template void demultiply_alpha<image_data_any> (image_data_any &);

// Temporary, can be removed once image_view_any and image_data_any are the only ones passed
template <>
MAPNIK_DECL void demultiply_alpha<image_data_rgba8>(image_data_rgba8 & image)
{
    detail::demultiply_visitor visit;
    visit(image);
}

template <typename T>
MAPNIK_DECL void set_premultiplied_alpha(T & image, bool status)
{
    util::apply_visitor(detail::set_premultiplied_visitor(status), image);
}

template void set_premultiplied_alpha<image_data_any> (image_data_any &, bool);

// Temporary, can be removed once image_view_any and image_data_any are the only ones passed
template <>
MAPNIK_DECL void set_premultiplied_alpha<image_data_rgba8>(image_data_rgba8 & image, bool status)
{
    detail::set_premultiplied_visitor visit(status);
    visit(image);
}

} // end ns
