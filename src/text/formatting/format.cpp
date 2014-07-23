/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2012 Artem Pavlenko
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
#include <mapnik/debug.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/text/formatting/format.hpp>
#include <mapnik/text/properties_util.hpp>
#include <mapnik/ptree_helpers.hpp>
#include <mapnik/xml_node.hpp>

//boost

#include <boost/property_tree/ptree.hpp>

namespace mapnik { namespace formatting {

using boost::property_tree::ptree;

void format_node::to_xml(ptree & xml) const
{
    ptree & new_node = xml.push_back(ptree::value_type("Format", ptree()))->second;
    if (face_name) set_attr(new_node, "face-name", *face_name);

    if (text_size) serialize_property("size", *text_size, xml);
    if (character_spacing) serialize_property("character-spacing", *character_spacing, xml);

    if (line_spacing) serialize_property("line-spacing", *line_spacing, xml);
    if (text_opacity) serialize_property("opacity", *text_opacity, xml);

    if (wrap_before) set_attr(new_node, "wrap-before", *wrap_before); // FIXME!!!

    if (wrap_char) serialize_property("wrap_char", *wrap_char, xml);

    if (text_transform) set_attr(new_node, "text-transform", *text_transform);
    if (fill) set_attr(new_node, "fill", *fill);
    if (halo_fill) set_attr(new_node, "halo-fill", *halo_fill);

    if (halo_radius) serialize_property("halo-radius", *halo_radius, xml);

    if (child_) child_->to_xml(new_node);
}


node_ptr format_node::from_xml(xml_node const& xml)
{
    format_node *n = new format_node();
    node_ptr np(n);

    node_ptr child = node::from_xml(xml);
    n->set_child(child);

    n->face_name = xml.get_opt_attr<std::string>("face-name");
    //TODO: Fontset is problematic. We don't have the fontsets pointer here...
    // exprs
    set_property_from_xml<double>(n->text_size, "size", xml);
    set_property_from_xml<double>(n->character_spacing, "character-spacing", xml);
    set_property_from_xml<double>(n->line_spacing, "line-spacing", xml);
    set_property_from_xml<double>(n->text_opacity, "opacity", xml);
    //set_property_from_xml<double>(n->halo_opacity, "halo-opacity", xml); FIXME
    set_property_from_xml<double>(n->halo_radius, "halo-radius", xml);
    set_property_from_xml<std::string>(n->wrap_char, "wrap-character", xml);
    //
    n->text_transform = xml.get_opt_attr<text_transform_e>("text-transform");
    n->fill = xml.get_opt_attr<color>("fill");
    n->halo_fill = xml.get_opt_attr<color>("halo-fill");

    return np;
}


void format_node::apply(char_properties_ptr p, feature_impl const& feature, attributes const& attrs, text_layout &output) const
{
    char_properties_ptr new_properties = std::make_shared<char_properties>(*p);

    if (text_size) new_properties->text_size = boost::apply_visitor(extract_value<value_double>(feature,attrs), *text_size);
    if (character_spacing) new_properties->character_spacing = boost::apply_visitor(extract_value<value_double>(feature,attrs), *character_spacing);
    if (line_spacing) new_properties->line_spacing = boost::apply_visitor(extract_value<value_double>(feature,attrs), *line_spacing);
    if (text_opacity) new_properties->text_opacity = boost::apply_visitor(extract_value<value_double>(feature,attrs), *text_opacity);

    if (wrap_char)
    {
        std::string str = boost::apply_visitor(extract_value<std::string>(feature,attrs), *wrap_char);
        if (!str.empty())
        {
            new_properties->wrap_char = str[0];
        }
    }
    if (halo_radius) new_properties->halo_radius = boost::apply_visitor(extract_value<value_double>(feature,attrs), *halo_radius);

    if (face_name) new_properties->face_name = *face_name;
    if (text_transform) new_properties->text_transform = *text_transform;
    if (fill) new_properties->fill = *fill;
    if (halo_fill) new_properties->halo_fill = *halo_fill;

    if (child_) child_->apply(new_properties, feature, attrs, output);
    else MAPNIK_LOG_WARN(format) << "Useless format: No text to format";
}


void format_node::set_child(node_ptr child)
{
    child_ = child;
}


node_ptr format_node::get_child() const
{
    return child_;
}

void format_node::add_expressions(expression_set & output) const
{
    if (child_) child_->add_expressions(output);
}

} //ns formatting
} //ns mapnik
