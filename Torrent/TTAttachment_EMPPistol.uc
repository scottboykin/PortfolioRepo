class TTAttachment_EMPPistol extends UTWeaponAttachment;

defaultproperties
{
	// Weapon SkeletalMesh
	Begin Object Name=SkeletalMeshComponent0
		SkeletalMesh=SkeletalMesh'TT_Weapons.SK_mesh.pistol_mesh_final_01'
		Rotation = (pitch=0,yaw=16384,roll=0)
		Translation = (x=0, y=2, z=-5)
	End Object

	WeapAnimType=EWAT_Pistol

	MuzzleFlashSocket=MuzzleFlashSocket

	MuzzleFlashPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_01'
	MuzzleFlashAltPSCTemplate=ParticleSystem'TT_Particles.Particles.muzzle_flash_01'
	MuzzleFlashDuration=0.33;
	
	WeaponClass=class'TTWeap_EMPPistol'
}

