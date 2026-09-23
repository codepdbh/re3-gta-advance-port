#ifndef __GTA_ANIMBLENDASSOCGROUP_H__
#define __GTA_ANIMBLENDASSOCGROUP_H__

class CAnimBlendAssociation;

class CAnimBlendAssocGroup
{
public:
	CAnimBlendAssociation *assocList;
	int32 numAssociations;

	CAnimBlendAssocGroup(void);
	~CAnimBlendAssocGroup(void);
	void DestroyAssociations(void);
	CAnimBlendAssociation *GetAnimation(uint32 id);
	CAnimBlendAssociation *GetAnimation(const char *name);
	CAnimBlendAssociation *CopyAnimation(uint32 id);
	CAnimBlendAssociation *CopyAnimation(const char *name);
	void CreateAssociations(const char *name);
	void CreateAssociations(const char *blockName, RpClump *clump, const char **animNames, int numAssocs);
};

#endif // __GTA_ANIMBLENDASSOCGROUP_H__
