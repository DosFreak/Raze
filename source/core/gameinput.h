#pragma once

#include "m_fixed.h"
#include "binaryangle.h"
#include "gamecvars.h"
#include "packet.h"

int getincangle(int a, int na);
double getincanglef(double a, double na);
fixed_t getincangleq16(fixed_t a, fixed_t na);
lookangle getincanglebam(binangle a, binangle na);

struct PlayerHorizon
{
	fixedhoriz horiz, ohoriz, horizoff, ohorizoff, target;
	double adjustment;

	void backup()
	{
		ohoriz = horiz;
		ohorizoff = horizoff;
	}

	void restore()
	{
		horiz = ohoriz;
		horizoff = ohorizoff;
	}

	void addadjustment(double value)
	{
		if (!SyncInput())
		{
			adjustment += value * FRACUNIT;
		}
		else
		{
			horiz += q16horiz(FloatToFixed(value));
		}
	}

	void resetadjustment()
	{
		adjustment = 0;
	}

	void settarget(int value, bool backup = false)
	{
		if (!SyncInput() && !backup)
		{
			target = buildhoriz(value);
			if (target.asq16() == 0) target = q16horiz(1);
		}
		else
		{
			horiz = buildhoriz(value);
			if (backup) ohoriz = horiz;
		}
	}

	void settarget(double value, bool backup = false)
	{
		if (!SyncInput() && !backup)
		{
			target = buildfhoriz(value);
			if (target.asq16() == 0) target = q16horiz(1);
		}
		else
		{
			horiz = buildfhoriz(value);
			if (backup) ohoriz = horiz;
		}
	}

	void settarget(fixedhoriz value, bool backup = false)
	{
		if (!SyncInput() && !backup)
		{
			target = value;
			if (target.asq16() == 0) target = q16horiz(1);
		}
		else
		{
			horiz = value;
			if (backup) ohoriz = horiz;
		}
	}

	void processhelpers(double const scaleAdjust)
	{
		if (target.asq16())
		{
			horiz += q16horiz(xs_CRoundToInt(scaleAdjust * (target - horiz).asq16()));

			if (abs((horiz - target).asq16()) < FRACUNIT)
			{
				horiz = target;
				target = q16horiz(0);
			}
		}
		else if (adjustment)
		{
			horiz += q16horiz(xs_CRoundToInt(scaleAdjust * adjustment));
		}
	}

	fixedhoriz osum()
	{
		return ohoriz + ohorizoff;
	}

	fixedhoriz sum()
	{
		return horiz + horizoff;
	}

	fixedhoriz interpolatedsum(double const smoothratio)
	{
		double const ratio = smoothratio * (1. / FRACUNIT);
		fixed_t const prev = osum().asq16();
		fixed_t const curr = sum().asq16();
		return q16horiz(prev + xs_CRoundToInt(ratio * (curr - prev)));
	}
};

struct PlayerAngle
{
	binangle ang, oang, target;
	lookangle look_ang, olook_ang, rotscrnang, orotscrnang, spin;
	double adjustment;

	void backup()
	{
		oang = ang;
		olook_ang = look_ang;
		orotscrnang = rotscrnang;
	}

	void restore()
	{
		ang = oang;
		look_ang = olook_ang;
		rotscrnang = orotscrnang;
	}

	void addadjustment(int value)
	{
		if (!SyncInput())
		{
			adjustment += BAngToBAM(value);
		}
		else
		{
			ang += buildang(value);
		}
	}

	void addadjustment(double value)
	{
		if (!SyncInput())
		{
			adjustment += value * BAMUNIT;
		}
		else
		{
			ang += bamang(xs_CRoundToUInt(value * BAMUNIT));
		}
	}

	void addadjustment(lookangle value)
	{
		if (!SyncInput())
		{
			adjustment += value.asbam();
		}
		else
		{
			ang += bamang(value.asbam());
		}
	}

	void addadjustment(binangle value)
	{
		if (!SyncInput())
		{
			adjustment += value.asbam();
		}
		else
		{
			ang += value;
		}
	}

	void resetadjustment()
	{
		adjustment = 0;
	}

	void settarget(int value, bool backup = false)
	{
		if (!SyncInput() && !backup)
		{
			target = buildang(value & 2047);
			if (target.asbam() == 0) target = bamang(1);
		}
		else
		{
			ang = buildang(value & 2047);
			if (backup) oang = ang;
		}
	}

	void settarget(double value, bool backup = false)
	{
		if (!SyncInput() && !backup)
		{
			target = buildfang(fmod(value, 2048));
			if (target.asbam() == 0) target = bamang(1);
		}
		else
		{
			ang = buildfang(fmod(value, 2048));
			if (backup) oang = ang;
		}
	}

	void settarget(binangle value, bool backup = false)
	{
		if (!SyncInput() && !backup)
		{
			target = value;
			if (target.asbam() == 0) target = bamang(1);
		}
		else
		{
			ang = value;
			if (backup) oang = ang;
		}
	}

	void processhelpers(double const scaleAdjust)
	{
		if (target.asbam())
		{
			ang += bamang(xs_CRoundToUInt(scaleAdjust * getincanglebam(ang, target).asbam()));

			if (getincanglebam(ang, target).asbam() < BAMUNIT)
			{
				ang = target;
				target = bamang(0);
			}
		}
		else if (adjustment)
		{
			ang += bamang(xs_CRoundToUInt(scaleAdjust * adjustment));
		}
	}

	binangle osum()
	{
		return oang + olook_ang;
	}

	binangle sum()
	{
		return ang + look_ang;
	}

	binangle interpolatedsum(double const smoothratio)
	{
		double const ratio = smoothratio * (1. / FRACUNIT);
		uint32_t const dang = UINT32_MAX >> 1;
		int64_t const prev = osum().asbam();
		int64_t const curr = sum().asbam();
		return bamang(prev + xs_CRoundToUInt(ratio * (((curr + dang - prev) & 0xFFFFFFFF) - dang)));
	}

	lookangle interpolatedlookang(double const smoothratio)
	{
		double const ratio = smoothratio * (1. / FRACUNIT);
		return bamlook(olook_ang.asbam() + xs_CRoundToInt(ratio * (look_ang - olook_ang).asbam()));
	}

	lookangle interpolatedrotscrn(double const smoothratio)
	{
		double const ratio = smoothratio * (1. / FRACUNIT);
		return bamlook(orotscrnang.asbam() + xs_CRoundToInt(ratio * (rotscrnang - orotscrnang).asbam()));
	}

	double look_anghalf(double const smoothratio)
	{
		return (!SyncInput() ? look_ang : interpolatedlookang(smoothratio)).asbam() * (0.5 / BAMUNIT); // Used within draw code for weapon and crosshair when looking left/right.
	}
};

class FSerializer;
FSerializer& Serialize(FSerializer& arc, const char* keyname, PlayerAngle& w, PlayerAngle* def);
FSerializer& Serialize(FSerializer& arc, const char* keyname, PlayerHorizon& w, PlayerHorizon* def);


void updateTurnHeldAmt(double const scaleAdjust);
bool const isTurboTurnTime();
void resetTurnHeldAmt();
void processMovement(InputPacket* currInput, InputPacket* inputBuffer, ControlInfo* const hidInput, double const scaleAdjust, int const drink_amt = 0, bool const allowstrafe = true, double const turnscale = 1);
void sethorizon(fixedhoriz* horiz, float const horz, ESyncBits* actions, double const scaleAdjust = 1);
void applylook(PlayerAngle* angle, float const avel, ESyncBits* actions, double const scaleAdjust = 1);
void calcviewpitch(vec2_t const pos, fixedhoriz* horizoff, binangle const ang, bool const aimmode, bool const canslopetilt, int const cursectnum, double const scaleAdjust = 1, bool const climbing = false);
