#include "common.h"

void
CVector::Normalise(void)
{
	float sq = MagnitudeSqr();
	if (sq > 0.0f) {
		float invsqrt = RecipSqrt(sq);
		x *= invsqrt;
		y *= invsqrt;
		z *= invsqrt;
	} else
		x = 1.0f;
}

CVector
CrossProduct(const CVector &v1, const CVector &v2)
{
	return CVector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

CVector
Multiply3x3(const CMatrix &mat, const CVector &vec)
{
#ifdef GTA_PS2
	CVector out;
	__asm__ __volatile__("\n\
		.set noreorder\n\
		ldr	$8,0(%2)\n\
		ldl	$8,7(%2)\n\
		lw	$9,8(%2)\n\
		pcpyld	$10,$9,$8\n\
		qmtc2	$10,vf01\n\
		lqc2	vf02,0x00(%1)\n\
		lqc2	vf03,0x10(%1)\n\
		lqc2	vf04,0x20(%1)\n\
		vmulax.xyz	ACC,vf02,vf01\n\
		vmadday.xyz	ACC,vf03,vf01\n\
		vmaddz.xyz	vf06,vf04,vf01\n\
		qmfc2	$8,vf06\n\
		sw	$8,0(%0)\n\
		prot3w	$9,$8\n\
		sw	$9,4(%0)\n\
		prot3w	$8,$9\n\
		sw	$8,8(%0)\n\
		.set reorder\n\
		": : "r" (&out) , "r" (&mat), "r" (&vec)
		 : "$8", "$9", "$10", "memory");
	return out;
#else
	return CVector(mat.rx * vec.x + mat.fx * vec.y + mat.ux * vec.z,
	               mat.ry * vec.x + mat.fy * vec.y + mat.uy * vec.z,
	               mat.rz * vec.x + mat.fz * vec.y + mat.uz * vec.z);
#endif
}

CVector
Multiply3x3(const CVector &vec, const CMatrix &mat)
{
	return CVector(mat.rx * vec.x + mat.ry * vec.y + mat.rz * vec.z,
	               mat.fx * vec.x + mat.fy * vec.y + mat.fz * vec.z,
	               mat.ux * vec.x + mat.uy * vec.y + mat.uz * vec.z);
}

CVector
operator*(const CMatrix &mat, const CVector &vec)
{
#ifdef GTA_PS2
	CVector out;
	__asm__ __volatile__("\n\
		.set noreorder\n\
		ldr	$8,0(%2)\n\
		ldl	$8,7(%2)\n\
		lw	$9,8(%2)\n\
		pcpyld	$10,$9,$8\n\
		qmtc2	$10,vf01\n\
		lqc2	vf02,0x00(%1)\n\
		lqc2	vf03,0x10(%1)\n\
		lqc2	vf04,0x20(%1)\n\
		lqc2	vf05,0x30(%1)\n\
		vmulax.xyz	ACC,vf02,vf01\n\
		vmadday.xyz	ACC,vf03,vf01\n\
		vmaddaz.xyz	ACC,vf04,vf01\n\
		vmaddw.xyz	vf06,vf05,vf00\n\
		qmfc2	$8,vf06\n\
		sw	$8,0(%0)\n\
		prot3w	$9,$8\n\
		sw	$9,4(%0)\n\
		prot3w	$8,$9\n\
		sw	$8,8(%0)\n\
		.set reorder\n\
		": : "r" (&out) , "r" (&mat), "r" (&vec)
		 : "$8", "$9", "$10", "memory");
	return out;
#else
	return CVector(mat.rx * vec.x + mat.fx * vec.y + mat.ux * vec.z + mat.px,
	               mat.ry * vec.x + mat.fy * vec.y + mat.uy * vec.z + mat.py,
	               mat.rz * vec.x + mat.fz * vec.y + mat.uz * vec.z + mat.pz);
#endif
}
