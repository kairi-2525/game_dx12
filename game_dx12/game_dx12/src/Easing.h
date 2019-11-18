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
namespace
{
	template<typename _Ty>
	constexpr _Ty _0 = static_cast<_Ty>(0.f);
	template<typename _Ty>
	constexpr _Ty _0_5 = static_cast<_Ty>(0.5f);
	template<typename _Ty>
	constexpr _Ty _0_75 = static_cast<_Ty>(0.75f);
	template<typename _Ty>
	constexpr _Ty _0_9375 = static_cast<_Ty>(0.9375f);
	template<typename _Ty>
	constexpr _Ty _0_984375 = static_cast<_Ty>(0.984375f);

	template<typename _Ty>
	constexpr _Ty _1 = static_cast<_Ty>(1.f);
	template<typename _Ty>
	constexpr _Ty _1_5 = static_cast<_Ty>(1.5f);
	template<typename _Ty>
	constexpr _Ty _1_525 = static_cast<_Ty>(1.525f);

	template<typename _Ty>
	constexpr _Ty _2 = static_cast<_Ty>(2.f);
	template<typename _Ty>
	constexpr _Ty _2_25 = static_cast<_Ty>(2.25f);
	template<typename _Ty>
	constexpr _Ty _2_5 = static_cast<_Ty>(2.5f);
	template<typename _Ty>
	constexpr _Ty _2_625 = static_cast<_Ty>(2.625f);
	template<typename _Ty>
	constexpr _Ty _2_75 = static_cast<_Ty>(2.75f);

	template<typename _Ty>
	constexpr _Ty _7_5625 = static_cast<_Ty>(7.5625f);

	template<typename _Ty>
	constexpr _Ty _10 = static_cast<_Ty>(10.f);

	template<typename _Ty>
	constexpr _Ty _90 = static_cast<_Ty>(90.f);
}

//---------------------------------------------------------
//!+++  関数
//---------------------------------------------------------

// イージング関数
namespace Easing
{
	// https://easings.net/  イージング関数一覧
	// https://qiita.com/hart_edsf/items/962ac03281b871dcc0df  引用サイト（2019/11/18現在、削除されている模様）

	template<typename _Ty = float>
	static inline _Ty InQuad(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return max * time * time + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutQuad(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return -max * time * (time - _2<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutQuad(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		if (time / _2<_Ty> < _1<_Ty>)
			return max / _2<_Ty> * time * time + min;

		--time;

		return -max * (time * (time - _2<_Ty>) - _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InCubic(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * time + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutCubic(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time = time / totaltime - _1<_Ty>;

		return max * (time * time * time + _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutCubic(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		if (time / _2<_Ty> < _1<_Ty>)
			return max / _2<_Ty> * time * time * time + min;

		time -= _2<_Ty>;

		return max / _2<_Ty> * (time * time * time + _2<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InQuart(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * time * time + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutQuart(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time = time / totaltime - _1<_Ty>;

		return -max * (time * time * time * time - _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutQuart(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		if (time / _2<_Ty> < _1<_Ty>)
			return max / _2<_Ty> * time * time * time * time + min;

		time -= _2<_Ty>;

		return -max / _2<_Ty> * (time * time * time * time - _2<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InQuint(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * time * time * time + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutQuint(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time = time / totaltime - _1<_Ty>;

		return max * (time * time * time * time * time + _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutQuint(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		if (time / _2<_Ty> < _1<_Ty>)
			return max / _2<_Ty> * time * time * time * time * time + min;

		time -= _2<_Ty>;

		return max / _2<_Ty> * (time * time * time * time * time + _2<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InSine(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;

		return -max * std::cos(time * Math::ToRadian(_90<_Ty>) / totaltime) + max + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutSine(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;

		return max * std::sin(time * Math::ToRadian(_90<_Ty>) / totaltime) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutSine(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;

		return -max / _2<_Ty> * (std::cos(time * Math::PAI<_Ty> / totaltime) - 1) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InExp(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;

		return  Math::AdjEqual<_Ty>(time, _0<_Ty>) ?
			min :
			max * std::pow(_2<_Ty>, _10<_Ty> * (time / totaltime - _1<_Ty>)) + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutExp(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;

		return Math::AdjEqual<_Ty>(time, totaltime) ?
			max + min :
			max * (-std::pow(_2<_Ty>, -_10<_Ty> * time / totaltime) + _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutExp(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		if (Math::AdjEqual<_Ty>(time, _0<_Ty>))
			return min;

		if (Math::AdjEqual<_Ty>(time, totaltime))
			return max;

		max -= min;
		time /= totaltime;

		if (time / _2<_Ty> < _1<_Ty>)
			return max / _2<_Ty> * std::pow(_2<_Ty>, _10<_Ty> * (time - _1<_Ty>)) + min;

		--time;

		return max / _2<_Ty> * (-std::pow(_2<_Ty>, -_10<_Ty> * time) + _2<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InCirc(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return -max * (std::sqrt(_1<_Ty> -time * time) - _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutCirc(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time = time / totaltime - _1<_Ty>;

		return max * std::sqrt(_1<_Ty> -time * time) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutCirc(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		if (time / _2<_Ty> < _1<_Ty>)
			return -max / _2<_Ty> * (std::sqrt(_1<_Ty> -time * time) - _1<_Ty>) + min;

		time -= _2<_Ty>;

		return max / _2<_Ty> * (std::sqrt(_1<_Ty> -time * time) + _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InBack(_Ty time, _Ty totaltime, _Ty back, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		return max * time * time * ((back + _1<_Ty>) * time - back) + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutBack(_Ty time, _Ty totaltime, _Ty back, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time = time / totaltime - _1<_Ty>;

		return max * (time * time * ((back + _1<_Ty>) * time * back) + _1<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutBack(_Ty time, _Ty totaltime, _Ty back, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		back *= _1_525<_Ty>;

		if (time / _2<_Ty> < _1<_Ty>)
			return max * (time * time * ((back + _1<_Ty>) * time - back)) + min;

		time -= _2<_Ty>;

		return max / _2<_Ty> * (time * time * ((back + _1<_Ty>) * time + back) + _2<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty OutBounce(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		max -= min;
		time /= totaltime;

		if (time < _1<_Ty> / _2_75<_Ty>)
			return max * (_7_5625<_Ty> * time * time) + min;

		else if (time < _2<_Ty> / _2_75<_Ty>)
		{
			time -= _1_5<_Ty> / _2_75<_Ty>;

			return max * (_7_5625<_Ty> * time * time + _0_75<_Ty>) + min;
		}
		else if (time < _2_5<_Ty> / _2_75<_Ty>)
		{
			time -= _2_25<_Ty> / _2_75<_Ty>;

			return max * (_7_5625<_Ty> * time * time + _0_9375<_Ty>) + min;
		}
		else
		{
			time -= _2_625<_Ty> / _2_75<_Ty>;

			return max * (_7_5625<_Ty> * time * time + _0_984375<_Ty>) + min;
		}
	}

	template<typename _Ty = float>
	static inline _Ty InBounce(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		return max - OutBounce<_Ty>(totaltime - time, totaltime, max - min, _0<_Ty>) + min;
	}

	template<typename _Ty = float>
	static inline _Ty InOutBounce(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		if (time < totaltime / _2<_Ty>)
		{
			return InBounce<_Ty>(time * _2<_Ty>, totaltime, max - min, max) * _0_5<_Ty> +min;
		}
		else
		{
			return OutBounce<_Ty>(time * _2<_Ty> -totaltime, totaltime, max - min, _0<_Ty>) * _0_5<_Ty> +min + (max - min) * _0_5<_Ty>;
		}
	}

	template<typename _Ty = float>
	static inline _Ty Linear(_Ty time, _Ty totaltime, _Ty max = _1<_Ty>, _Ty min = _0<_Ty>)
	{
		return (max - min) * time / totaltime + min;
	}
}