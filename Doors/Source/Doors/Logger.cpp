#include "Logger.h"

// Log CString

void Logger::Log(bool Show, const char *Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor, Msg, true, FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("%s"), ANSI_TO_TCHAR(Msg));
}

// Log String

void Logger::Log(bool Show, const FString &Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor, Msg, true, FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
}

// Log Text

void Logger::Log(bool Show, const FText &Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor, Msg.ToString(), true,
                                     FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg.ToString());
}

// Log Bool

void Logger::Log(bool Show, bool Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor, FString::FromInt(Msg), true,
                                     FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("%d"), Msg);
}

// Log Int

void Logger::Log(bool Show, int Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor, FString::FromInt(Msg), true,
                                     FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("%d"), Msg);
}

// Log Float

void Logger::Log(bool Show, float Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor, FString::SanitizeFloat(Msg), true,
                                     FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("%f"), Msg);
}

// Log FVector

void Logger::Log(bool Show, const FVector &Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(Key, Duration, DisplayColor,
                                     FString::Printf(TEXT("(X:%f, Y:%f, Z:%f)"), Msg.X, Msg.Y, Msg.Z), true,
                                     FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("(X:%f, Y:%f, Z:%f)"), Msg.X, Msg.Y, Msg.Z);
}

// Log Rotator

void Logger::Log(bool Show, const FRotator &Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(
        Key, Duration, DisplayColor,
        FString::Printf(TEXT("(Pitch:%f, Yaw:%f, Roll:%f)"), Msg.Pitch, Msg.Yaw, Msg.Roll), true,
        FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("(Pitch:%f, Yaw:%f, Roll:%f)"), Msg.Pitch, Msg.Yaw, Msg.Roll);
}

// Log Quat

void Logger::Log(bool Show, const FQuat &Msg, float Duration, const FColor &DisplayColor, int32 Key)
{
    if (!GEngine || !Show)
        return;

    GEngine->AddOnScreenDebugMessage(
        Key, Duration, DisplayColor,
        FString::Printf(TEXT("(X:%f, Y:%f, Z:%f, W:%f)"), Msg.X, Msg.Y, Msg.Z, Msg.W), true,
        FVector2D::UnitVector);

    UE_LOG(LogTemp, Warning, TEXT("(X:%f, Y:%f, Z:%f, W:%f)"), Msg.X, Msg.Y, Msg.Z, Msg.W);
}
