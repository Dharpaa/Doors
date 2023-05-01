#pragma once

#include "CoreMinimal.h"

static class Logger
{
  public:
    // Log CString

    static void Log(bool Show, const char *Msg, float Duration = 1.f,
                    const FColor &DisplayColor = FColor::Cyan, int32 Key = -1);

    // Log String

    static void Log(bool Show, const FString &Msg, float Duration = 1.f,
                    const FColor &DisplayColor = FColor::Cyan, int32 Key = -1);

    // Log Text

    static void Log(bool Show, const FText &Msg, float Duration = 1.f,
                    const FColor &DisplayColor = FColor::Cyan, int32 Key = -1);

    // Log Bool

    static void Log(bool Show, bool Msg, float Duration = 1.f, const FColor &DisplayColor = FColor::Cyan,
                    int32 Key = -1);

    // Log Int

    static void Log(bool Show, int Msg, float Duration = 1.f, const FColor &DisplayColor = FColor::Cyan,
                    int32 Key = -1);

    // Log Float

    static void Log(bool Show, float Msg, float Duration = 1.f, const FColor &DisplayColor = FColor::Cyan,
                    int32 Key = -1);

    // Log FVector

    static void Log(bool Show, const FVector &Msg, float Duration = 1.f,
                    const FColor &DisplayColor = FColor::Cyan, int32 Key = -1);

    // Log Rotator

    static void Log(bool Show, const FRotator &Msg, float Duration = 1.f,
                    const FColor &DisplayColor = FColor::Cyan, int32 Key = -1);

    // Log Quat

    static void Log(bool Show, const FQuat &Msg, float Duration = 1.f,
                    const FColor &DisplayColor = FColor::Cyan, int32 Key = -1);

    // Log Enum

    template <class T>
    static void LogEnum(bool Show, const T &Msg, float Duration = 1.f,
                        const FColor &DisplayColor = FColor::Cyan, int32 Key = -1)
    {
        if (!GEngine || !Show)
            return;

        GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor,
                                         UEnum::GetDisplayValueAsText(Msg).ToString(), true,
                                         FVector2D::UnitVector);
    }
};
