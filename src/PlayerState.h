#pragma once

enum class PlayerState
{
    Idle,
    CardReaderInitializing,
    CardReaderError,
    CardReaderTagReading,
    CardReaderAvailable,
    CommandProcessing,
    PlayingTag

};
