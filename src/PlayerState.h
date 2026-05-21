#pragma once

enum class PlayerState
{
    Startup,
    Idle,
    CardReaderInitializing,
    CardReaderError,
    CardReaderTagReading,
    CardReaderTagAvailable,
    CommandProcessing,
    SettingVolume,
    PlayingTag,
    PlayingNotPossible
};
