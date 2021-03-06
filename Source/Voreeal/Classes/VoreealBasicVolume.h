#pragma once

#include <PolyVox/MaterialDensityPair.h>
#include <PolyVox/RawVolume.h>

#include "VoreealVolume.h"
#include "VoreealBasicVolume.generated.h"

USTRUCT()
struct FBasicVolumeSocket
{
	GENERATED_BODY()

	// Transform in pivot space (*not* texture space)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sockets")
	FTransform LocalTransform;

	// Name of the socket
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sockets")
	FName SocketName;
};

/**
 * Basic Voxel Volume; Enclosed volume with just basic LOD.
 */
UCLASS(Blueprintable, meta = (DisplayThumbnail = "true"))
class VOREEAL_API UBasicVolume 
	: public UVoreealVolume
	, public VoreealExtractHelper<PolyVox::RawVolume<PolyVox::MaterialDensityPair88>, PolyVox::MaterialDensityPair88>
{
	GENERATED_BODY()

public:
	// List of sockets on this volume.
	UPROPERTY(Category = Sprite, EditAnywhere)
	TArray<FBasicVolumeSocket> Sockets;

public:
	UBasicVolume(const class FObjectInitializer& ObjectInitializer);

	// Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const UProperty* InProperty) const override;
#endif
#if WITH_EDITORONLY_DATA
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif
	// End UObject Interface

	// Begin UVoreealVolume Interface
	virtual bool IsValid() const override;
	virtual void GetVoxel(const FVector& Location, uint8& Material, uint8& Density) override;
	virtual FVoreealMesh ExtractMesh(const FVoreealExtractorOptions& Options) override;
	// End UVoreealVolume Interface

	// Gets the enclosing region.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voreeal")
	FVoreealRegion GetEnclosingRegion() const;

	// Resize the volume.
	UFUNCTION(BlueprintCallable, Category = "Voreeal")
	void Resize(const FIntVector& NewSize);

	// Resize the volume.
	UFUNCTION(BlueprintCallable, Category = "Voreeal")
	void ResizeRegion(const FVoreealRegion& NewRegion);

	// Pick in local volume space.
	virtual bool PickFirstSolidVoxel(const FVector& Start, const FVector& End, FIntVector& HitPoint) const;

	// Pick in local volume space.
	virtual bool PickLastSolidVoxel(const FVector& Start, const FVector& End, FIntVector& HitPoint) const;

public:
#if WITH_EDITORONLY_DATA
	// Importing data and options used
	UPROPERTY(Category = ImportSettings, VisibleAnywhere, Instanced)
	class UAssetImportData* AssetImportData;
#endif

#if WITH_EDITOR
	// Make sure all socket names are valid
	// All duplicate / empty names will be made unique
	void ValidateSocketNames();

	// Removes the specified socket
	void RemoveSocket(FName SocketName);
#endif

	// Search for a socket (note: do not cache this pointer; it's unsafe if the Socket array is edited)
	FBasicVolumeSocket* FindSocket(FName SocketName);

	// Returns true if the sprite has any sockets
	bool HasAnySockets() const { return Sockets.Num() > 0; }

	// Returns a list of all of the sockets
	void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const;

protected:
	virtual bool Internal_SetVoxel(FVector Location, const uint8& Material, const uint8& Density) override;
	virtual void Internal_SetSize(const FVoreealRegion& Region, bool New) override;

private:
	TSharedPtr<VolumeType> Volume;

	UPROPERTY()
	TArray<uint8> ImportedData;

	// UVoreealVolumeImporterFactory has to access ImportedData
	friend class UVoreealVolumeImporterFactory;
};
