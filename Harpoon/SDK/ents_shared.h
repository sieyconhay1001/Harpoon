typedef intp SerializedEntityHandle_t;

enum
{
	SERIALIZED_ENTITY_HANDLE_INVALID = (SerializedEntityHandle_t)0,
};

abstract_class ISerializedEntities
{
public:
	virtual SerializedEntityHandle_t AllocateSerializedEntity( char const *pFile, int nLine ) = 0;
	virtual void ReleaseSerializedEntity( SerializedEntityHandle_t handle ) = 0;
	virtual SerializedEntityHandle_t CopySerializedEntity( SerializedEntityHandle_t handle, char const *pFile, int nLine ) = 0;
};

extern ISerializedEntities *g_pSerializedEntities;

enum
{
	ENTITY_SENTINEL = 9999	// larger number than any real entity number
};

// Used to classify entity update types in DeltaPacketEntities.
enum UpdateType
{
	EnterPVS = 0,	// Entity came back into pvs, create new entity if one doesn't exist

	LeavePVS,		// Entity left pvs

	DeltaEnt,		// There is a delta for this entity.
	PreserveEnt,	// Entity stays alive but no delta ( could be LOD, or just unchanged )

	Finished,		// finished parsing entities successfully
	Failed,			// parsing error occured while reading entities
};

// Flags for delta encoding header
enum
{
	FHDR_ZERO			= 0x0000,
	FHDR_LEAVEPVS		= 0x0001,
	FHDR_DELETE			= 0x0002,
	FHDR_ENTERPVS		= 0x0004,
};
