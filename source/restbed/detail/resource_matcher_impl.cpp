/*
 * Site: restbed.corvusoft.co.uk
 * Author: Ben Crowhurst
 *
 * Copyright (c) 2013 Restbed Core Development Team and Community Contributors
 *
 * This file is part of Restbed.
 *
 * Restbed is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Restbed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with Restbed.  If not, see <http://www.gnu.org/licenses/>.
 */

//System Includes
#include <map>
#include <regex>
#include <vector> 
#include <string>
#include <iostream> //debug

//Project Includes
#include "restbed/request.h"
#include "restbed/resource.h"
#include "restbed/detail/helpers/string.h"
#include "restbed/detail/resource_matcher_impl.h"

//External Includes

//System Namespaces
using std::map;
using std::regex;
using std::string;
using std::vector;

//Project Namespaces
using restbed::detail::helpers::String;

//External Namespaces

namespace restbed
{
    namespace detail
    {
        ResourceMatcherImpl::ResourceMatcherImpl( const Request& request ) : m_request( request)
        {
            //n/a
        }
        
        ResourceMatcherImpl::ResourceMatcherImpl( const ResourceMatcherImpl& original ) : m_request( original.m_request )
        {
            //n/a
        }
        
        ResourceMatcherImpl::~ResourceMatcherImpl( void )
        {
            //n/a
        }     
        
        bool ResourceMatcherImpl::operator ( )( const Resource& resource ) const
        {
            return compare_path( m_request, resource ) and compare_headers( m_request, resource );
        }

        regex ResourceMatcherImpl::parse_filter_definition( const string& filter ) const
        {
            string definition = filter;

            if ( definition.front( ) == '{' )
            {
                if ( definition.back( ) == '}' )
                { 
                    definition = String::trim( definition, "{" );
                    definition = String::trim( definition, "}" ); 
                    
                    auto segments = String::split( definition, ':' );

                    if ( segments.size( ) not_eq 2 )
                    {
                        std::cout << "invalid path parameter" << std::endl;
                    }
                    
                    string value = String::trim( segments[ 1 ] );

                    definition = value;
                }
            }

            return regex( definition );  
        }

        bool ResourceMatcherImpl::compare_path( const Request& request, const Resource& resource ) const
        {
            bool result = true;

            auto path_filters = resource.get_path_filters( );

            auto path_segments = String::split( request.get_path( ), '/' );

            if ( path_segments.size( ) == path_filters.size( ) )
            {
                for ( vector< string >::size_type index = 0; index not_eq path_filters.size( ); index++ )
                {
                    regex pattern = parse_filter_definition( path_filters[ index ] );

                    if ( not regex_match( path_segments[ index ], pattern ) )
                    {
                        result = false;
                        break;
                    }
                }
            }

            return result;
        }

        bool ResourceMatcherImpl::compare_headers( const Request& request, const Resource& resource ) const
        {
            bool result = true;

            for ( auto filter : resource.get_header_filters( ) )
            {
                string name = filter.first;

                if ( request.has_header( name ) )
                {
                    string header = request.get_header( name );

                    regex pattern = regex( filter.second );

                    if ( not regex_match( header, pattern ) )
                    {
                        result = false;
                        break;
                    }
                }
                else
                {
                    result = false;
                    break;
                }
            }

            return result;
        }
    }
}
