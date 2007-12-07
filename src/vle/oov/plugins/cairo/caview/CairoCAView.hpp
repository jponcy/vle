/** 
 * @file CairoCAView.hpp
 * @brief 
 * @author The vle Development Team
 * @date Thu Oct 25 13:57:22 CEST 2007
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_OOV_PLUGINS_CAIRO_CAVIEW_HPP
#define VLE_OOV_PLUGINS_CAIRO_CAVIEW_HPP

#include <vle/oov/CairoPlugin.hpp>
#include <map>
#include <vector>
#include <boost/multi_array.hpp>

namespace vle { namespace oov { namespace plugin {

    class RealColor
    {
    public:
	enum color_type { LINEAR, HIGHVALUE, LOWVALUE };

	double m_min;
	double m_max;
	std::string m_color;
	color_type m_type;
	double m_coef;
	
	RealColor(double p_min, double p_max, const std::string & p_color, 
		  color_type p_type, double p_coef):
	    m_min(p_min), m_max(p_max), m_color(p_color),
	    m_type(p_type), m_coef(p_coef) { }
    };
	    
    class CairoCAView: public CairoPlugin
    {
    public:
	enum geometry { SQUARE, HEXA };
	enum type { INTEGER, REAL, BOOLEAN };

	class color { 
	public: 
	    int r; int g; int b; 
	    color(int _r = 0, int _g = 0, int _b = 0):r(_r), g(_g), b(_b) { } 
	};

	class cairo_color { 
	public: 
	    double r; double g; double b;
	    cairo_color(double _r = 0, double _g = 0, double _b = 0):r(_r), g(_g), b(_b) { } 
	};

	typedef std::map < std::string , std::pair < std::string , color > > ObjectList;

    public:
        CairoCAView(const std::string& location);

        virtual ~CairoCAView();

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close(const vpz::EndTrame& trame);

        virtual void preferredSize(int& width, int& height);

    private:
	cairo_color build_color(const std::string & p_value);
        void draw();
	void draw_hexa(Cairo::RefPtr < Cairo::Context > ctx, int x, int p_y);
	void draw_objects(Cairo::RefPtr < Cairo::Context > ctx);
	void draw_hexa_objects(Cairo::RefPtr < Cairo::Context > ctx);
	
	geometry mGeometry;
 	type mType;
 	std::string mCellName;
 	std::string mStateName;
 	std::map < int, color > mColorList;
 	std::list < RealColor > mRealColorList;
	ObjectList mObjectList;

	unsigned int mRows;
	unsigned int mColumns;

	int mWidth;
	int mHeight;
	int mMinX;
	int mMaxX;
	int mMinY;
	int mMaxY;
	int mStepX;
	int mStepX2;
	int mStepY;
	int mStepY3;
	int mStepY7;
	
	double mPen;

	int mWindowWidth;
	int mWindowHeight;

	double mTime;
	unsigned int mReceiveCell;
	unsigned int mReceiveObject;
	unsigned int mObjectNumber;

	typedef boost::multi_array < std::string, 2> array_type;
	typedef array_type::index index;
	array_type* mValues;

	typedef std::map < std::string, std::vector < std::pair < int, int > > > object_type;
	object_type mObjects;

        Cairo::RefPtr < Cairo::ImageSurface > mImageSurface;
    };
    
    DECLARE_OOV_PLUGIN(oov::plugin::CairoCAView);

}}} // namespace vle oov plugin

#endif