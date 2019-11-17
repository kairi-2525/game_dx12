//==================================================
//
//!+++ Easing.h
//
//==================================================
#pragma once

//---------------------------------------------------------
//!+++  インクルード
//---------------------------------------------------------
#include <cmath>
#include "Math.h"

//---------------------------------------------------------
//!+++  定数
//---------------------------------------------------------
template<typename _Ty>
constexpr _Ty _90 = static_cast<_Ty>(90.f);

//---------------------------------------------------------
//!+++  関数
//---------------------------------------------------------

// イージング関数
namespace Easing
{
	// https://easings.net/  イージング関数一覧
	// https://qiita.com/hart_edsf/items/962ac03281b871dcc0df  引用サイト（2019/11/18現在、削除されている模様）

	template<typename _Ty = float>
	inline _Ty InQuad(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return max * time * time + min;
	}

	template<typename _Ty = float>
	inline _Ty OutQuad(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return -max * time * (time - 2) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutQuad(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		if (time / 2 < 1)
			return max / 2 * time * time + min;

		--time;

		return -max * (time * (time - 2) - 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InCubic(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * time + min;
	}

	template<typename _Ty = float>
	inline _Ty OutCubic(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time = time / totaltime - 1;

		return max * (time * time * time + 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutCubic(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		if (time / 2 < 1)
			return max / 2 * time * time * time + min;

		time -= 2;

		return max / 2 * (time * time * time + 2) + min;
	}

	template<typename _Ty = float>
	inline _Ty InQuart(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * time * time + min;
	}

	template<typename _Ty = float>
	inline _Ty OutQuart(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time = time / totaltime - 1;

		return -max * (time * time * time * time - 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutQuart(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		if (time / 2 < 1)
			return max / 2 * time * time * time * time + min;

		time -= 2;

		return -max / 2 * (time * time * time * time - 2) + min;
	}

	template<typename _Ty = float>
	inline _Ty InQuint(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * time * time * time + min;
	}

	template<typename _Ty = float>
	inline _Ty OutQuint(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time = time / totaltime - 1;

		return max * (time * time * time * time * time + 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutQuint(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		if (time / 2 < 1)
			return max / 2 * time * time * time * time * time + min;

		time -= 2;

		return max / 2 * (time * time * time * time * time + 2) + min;
	}

	template<typename _Ty = float>
	inline _Ty InSine(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;

		return -max * std::cos<_Ty>(time * Math::ToRadian(_90<_Ty>) / totaltime) + max + min;
	}

	template<typename _Ty = float>
	inline _Ty OutSine(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;

		return max * std::sin<_Ty>(time * Math::ToRadian(_90<_Ty>) / totaltime) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutSine(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;

		return -max / 2 * (cos(time * Math::PAI<_Ty> / totaltime) - 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InExp(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;

		return time == 0.0 ? min : max * std::pow<_Ty>(2, 10 * (time / totaltime - 1)) + min;
	}

	template<typename _Ty = float>
	inline _Ty OutExp(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;

		return time == totaltime ? max + min : max * (-pow(2, -10 * time / totaltime) + 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutExp(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		if (time == 0.0)
			return min;

		if (time == totaltime)
			return max;

		max -= min;
		time /= totaltime;

		if (time / 2 < 1)
			return max / 2 * std::pow<_Ty>(2, 10 * (time - 1)) + min;

		--time;

		return max / 2 * (-std::pow<_Ty>(2, -10 * time) + 2) + min;
	}

	template<typename _Ty = float>
	inline _Ty InCirc(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return -max * (std::sqrt<_Ty>(1 - time * time) - 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty OutCirc(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time = time / totaltime - 1;

		return max * std::sqrt<_Ty>(1 - time * time) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutCirc(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		if (time / 2 < 1)
			return -max / 2 * (std::sqrt<_Ty>(1 - time * time) - 1) + min;

		time -= 2;

		return max / 2 * (std::sqrt<_Ty>(1 - time * time) + 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InBack(_Ty time, _Ty totaltime, _Ty back, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		return max * time * time *((back + 1) * time - back) + min;
	}

	template<typename _Ty = float>
	inline _Ty OutBack(_Ty time, _Ty totaltime, _Ty back, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time = time / totaltime - 1;

		return max * (time * time * ((back + 1) * time * back) + 1) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutBack(_Ty time, _Ty totaltime, _Ty back, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		back *= 1.525;

		if (time / 2 < 1)
			return max * (time * time * ((back + 1) * time - back)) + min;

		time -= 2;

		return max / 2 * (time * time * ((back + 1) * time + back) + 2) + min;
	}

	template<typename _Ty = float>
	inline _Ty OutBounce(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		max -= min;
		time /= totaltime;

		if (time < 1 / 2.75)
			return max * (7.5625 * time * time) + min;

		else if (time < 2 / 2.75)
		{
			time -= 1.5 / 2.75;

			return max * (7.5625 * time * time + 0.75) + min;
		}
		else if (time < 2.5 / 2.75)
		{
			time -= 2.25 / 2.75;

			return max * (7.5625 * time * time + 0.9375) + min;
		}
		else
		{
			time -= 2.625 / 2.75;

			return max * (7.5625 * time * time + 0.984375) + min;
		}
	}

	template<typename _Ty = float>
	inline _Ty InBounce(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		return max - OutBounce<_Ty>(totaltime - time, totaltime, max - min, 0) + min;
	}

	template<typename _Ty = float>
	inline _Ty InOutBounce(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		if (time < totaltime / 2)
		{
			return InBounce<_Ty>(time * 2, totaltime, max - min, max) * 0.5 + min;
		}
		else
		{
			return OutBounce<_Ty>(time * 2 - totaltime, totaltime, max - min, 0) * 0.5 + min + (max - min) * 0.5;
		}
	}

	template<typename _Ty = float>
	inline _Ty Linear(_Ty time, _Ty totaltime, _Ty max = 1.0, _Ty min = 0.0)
	{
		return (max - min) * time / totaltime + min;
	}
}