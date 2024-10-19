#pragma once
class Easing
{
public:
	static float Lerp(float start, float end, float t);

public:
	static float EaseInSine(float t);
	static float EaseOutSine(float t);
	static float EaseInOutSine(float t);

	static float EaseInCubic(float t);
	static float EaseOutCubic(float t);
	static float EaseInOutCubic(float t);

	static float EaseInQuint(float t);
	static float EaseOutQuint(float t);
	static float EaseInOutQuint(float t);

	static float EaseInCirc(float t);
	static float EaseOutCirc(float t);
	static float EaseInOutCirc(float t);

	static float EaseInElastic(float t);
	static float EaseOutElastic(float t);
	static float EaseInOutElastic(float t);

	static float EaseInQuad(float t);
	static float EaseOutQuad(float t);
	static float EaseInOutQuad(float t);

	static float EaseInQuart(float t);
	static float EaseOutQuart(float t);
	static float EaseInOutQuart(float t);

	static float EaseInExpo(float t);
	static float EaseOutExpo(float t);
	static float EaseInOutExpo(float t);

	static float EaseInBack(float t);
	static float EaseOutBack(float t);
	static float EaseInOutBack(float t);

	static float EaseInBounce(float t);
	static float EaseOutBounce(float t);
	static float EaseInOutBounce(float t);
};

