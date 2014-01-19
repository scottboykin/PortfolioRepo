class TTWeapon extends UTWeapon;

var( TTWeapon ) bool bHasRecoil;
var( TTWeapon ) float maxRecoilSpread;
var( TTWeapon ) float timeToStabilizeRecoil;
var( TTWeapon ) float timeToUnstablilizeRecoil;

var float recoilSpreadIncreaseRate;
var float recoilSpreadDecreaseRate;

simulated function FireAmmunition()
{
	super.FireAmmunition();

	if( bHasRecoil && Spread[0] != maxRecoilSpread )
	{
		Spread[0] += recoilSpreadIncreaseRate;
		if( Spread[0] > maxRecoilSpread )
		{   
			Spread[0] = maxRecoilSpread;
		}
	}
}

simulated function Projectile SpawnArchetypeProjectile( UTProjectile archetype )
{
	local vector		RealStartLoc;
	local Projectile	SpawnedProjectile;

	// tell remote clients that we fired, to trigger effects
	IncrementFlashCount();

	if( Role == ROLE_Authority )
	{
		// this is the location where the projectile is spawned.
		RealStartLoc = GetPhysicalFireStartLoc();

		// Spawn projectile
		SpawnedProjectile = Spawn(GetProjectileClass(), self,, RealStartLoc,,archetype); 
		if( SpawnedProjectile != None && !SpawnedProjectile.bDeleteMe )
		{
			SpawnedProjectile.Init( Vector(GetAdjustedAim( RealStartLoc )) );
		}

		// Return it up the line
		return SpawnedProjectile;
	}

	return None;
}

simulated function StopFire( byte FireModeNum )
{
	if( bHasRecoil )
	{
		Spread[0] = 0;
	}

	super.StopFire( FireModeNum );
}

DefaultProperties
{
	bHasRecoil = false
	maxRecoilSpread = 0
	recoilSpreadIncreaseRate = 0
	recoilSpreadDecreaseRate = 0

	WeaponEquipAnim = "shotgun_bringup"
	WeaponIdleAnims(0) = "shotgun_idle"
	WeaponPutDownAnim = "shotgun_putdown"
	WeaponFireAnim(0) = "shotgun_shoot"
	WeaponFireAnim(1) = "shotgun_shoot"
	
	PivotTranslation=( X=0, Y=0, Z=0 )
}
