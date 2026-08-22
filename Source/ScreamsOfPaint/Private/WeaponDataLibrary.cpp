#include "WeaponDataLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FString UWeaponDataLibrary::GetWeaponDataPath()
{
    return FPaths::ProjectSavedDir() / TEXT("WeaponConfig/range_weapons_data.json");
}

static void SanitizeJsonKeys(const TSharedPtr<FJsonObject>& Obj);

static void SanitizeJsonValue(const TSharedPtr<FJsonValue>& Value)
{
    if (!Value.IsValid()) return;

    if (Value->Type == EJson::Object)
    {
        SanitizeJsonKeys(Value->AsObject());
    }
    else if (Value->Type == EJson::Array)
    {
        for (const TSharedPtr<FJsonValue>& Elem : Value->AsArray())
            SanitizeJsonValue(Elem);
    }
}

static void SanitizeJsonKeys(const TSharedPtr<FJsonObject>& Obj)
{
    if (!Obj.IsValid()) return;

    TArray<TPair<FString, TSharedPtr<FJsonValue>>> Pairs;
    Pairs.Reserve(Obj->Values.Num());
    for (const auto& Pair : Obj->Values)
    {
        Pairs.Emplace(FString(FStringView(Pair.Key)), Pair.Value);
    }

    Obj->Values.Empty(Pairs.Num());

    for (TPair<FString, TSharedPtr<FJsonValue>>& Pair : Pairs)
    {
        SanitizeJsonValue(Pair.Value);
        Obj->SetField(Pair.Key.Replace(TEXT("_"), TEXT("")), Pair.Value);
    }
}

bool UWeaponDataLibrary::LoadRangeWeaponData(TArray<FRangeWeaponData>& OutData, FString& OutError)
{
    const FString Path = GetWeaponDataPath();

    FString JsonStr;
    if (!FFileHelper::LoadFileToString(JsonStr, *Path))
    {
        OutError = FString::Printf(TEXT("Khong doc duoc file: %s"), *Path);
        return false;
    }

    TArray<TSharedPtr<FJsonValue>> RootArray;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
    if (!FJsonSerializer::Deserialize(Reader, RootArray))
    {
        OutError = TEXT("Parse JSON that bai - kiem tra syntax (thieu dau phay?)");
        return false;
    }

    OutData.Reset();
    for (const TSharedPtr<FJsonValue>& Entry : RootArray)
    {
        const TSharedPtr<FJsonObject>* EntryObj = nullptr;
        if (!Entry.IsValid() || !Entry->TryGetObject(EntryObj)) continue;

        SanitizeJsonKeys(*EntryObj);

        FRangeWeaponData Data;
        if (FJsonObjectConverter::JsonObjectToUStruct(EntryObj->ToSharedRef(), &Data, 0, 0))
        {
            OutData.Add(MoveTemp(Data));
        }
    }

    OutError.Empty();
    return OutData.Num() > 0;
}