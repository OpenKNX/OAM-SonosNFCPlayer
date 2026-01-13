#pragma once

enum class PlayerState
{
    Idle,
    CardReaderInitializing,
    CardReaderError,
    CardReaderTagReading,
    CardReaderTagAvailable,
    CommandProcessing,
    SettingVolume,
    PlayingTag
};
