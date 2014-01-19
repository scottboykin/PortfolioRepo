class TTAttachment_MineLauncher extends UTWeaponAttachment;

defaultproperties
{
	// Weapon SkeletalMesh
	Begin Object Name=SkeletalMeshComponent0
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.shotgun_mesh_final_01'
		Rotation = (pitch=2,yaw=16384,roll=0)
		Translation = (x=0, y=2, z=-5)
	End Object

	BaseSkelComponent = SkeletalMeshComponent0

	MuzzleFlashSocket=MuzzleFlashSocket
	MuzzleFlashPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_03'
	MuzzleFlashAltPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_03'
	bMuzzleFlashPSCLoops=false
	MuzzleFlashLightClass=class'UTGame.UTLinkGunMuzzleFlashLight'

	MuzzleFlashDuration = 0.33
}

