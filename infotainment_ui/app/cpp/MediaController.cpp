// MediaController.cpp
// Media controller implementation

#include "MediaController.h"
#include <QTimer>

namespace automotive {
namespace infotainment {

MediaController::MediaController(QObject* parent)
    : QObject(parent)
{
    m_positionTimer = new QTimer(this);
    m_positionTimer->setInterval(1000);
    connect(m_positionTimer, &QTimer::timeout, this, &MediaController::onPositionTick);

    loadPlaylists();
    loadMockData();
    updateCurrentTrack();
}

MediaController::~MediaController() = default;

void MediaController::play()
{
    if (!m_playing) {
        m_playing = true;
        m_positionTimer->start();
        emit playingChanged(true);
    }
}

void MediaController::pause()
{
    if (m_playing) {
        m_playing = false;
        m_positionTimer->stop();
        emit playingChanged(false);
    }
}

void MediaController::togglePlayPause()
{
    if (m_playing) {
        pause();
    } else {
        play();
    }
}

void MediaController::next()
{
    if (!m_queue.isEmpty()) {
        m_position = 0;
        m_currentTrackIndex++;

        if (m_currentTrackIndex >= m_queue.size()) {
            if (m_repeatMode == 1) {  // Repeat all
                m_currentTrackIndex = 0;
            } else {
                m_currentTrackIndex = m_queue.size() - 1;
                pause();
            }
        }

        updateCurrentTrack();
        emit positionChanged(m_position);
    }
}

void MediaController::previous()
{
    if (m_position > 3000) {
        // Restart current track if > 3 seconds in
        m_position = 0;
        emit positionChanged(m_position);
    } else if (!m_queue.isEmpty()) {
        // Go to previous track
        m_position = 0;
        m_currentTrackIndex--;

        if (m_currentTrackIndex < 0) {
            m_currentTrackIndex = 0;
        }

        updateCurrentTrack();
        emit positionChanged(m_position);
    }
}

void MediaController::seek(int positionMs)
{
    m_position = qBound(0, positionMs, m_duration);
    emit positionChanged(m_position);
}

void MediaController::setSource(const QString& source)
{
    if (m_source != source) {
        m_source = source;
        emit sourceChanged(m_source);
    }
}

void MediaController::onPositionTick()
{
    if (m_playing) {
        m_position += 1000;
        if (m_position >= m_duration) {
            if (m_repeatMode == 2) {  // Repeat one
                m_position = 0;
                emit positionChanged(m_position);
            } else {
                next();
            }
        } else {
            emit positionChanged(m_position);
        }
    }
}

void MediaController::loadMockData()
{
    // Load tracks for current playlist
    m_queue.clear();

    auto addTrack = [this](const QString& title, const QString& artist,
                           const QString& album, int duration,
                           const QString& artColor, bool liked) {
        QVariantMap track;
        track[QStringLiteral("title")] = title;
        track[QStringLiteral("artist")] = artist;
        track[QStringLiteral("album")] = album;
        track[QStringLiteral("duration")] = duration;
        track[QStringLiteral("artColor")] = artColor;
        track[QStringLiteral("liked")] = liked;
        m_queue.append(track);
    };

    if (m_currentPlaylist == QStringLiteral("Liked Songs")) {
        addTrack(QStringLiteral("Driving Home"), QStringLiteral("Road Trip Band"),
                 QStringLiteral("Road Trip Anthems"), 195000, QStringLiteral("#1DB954"), true);
        addTrack(QStringLiteral("Highway Dreams"), QStringLiteral("The Cruisers"),
                 QStringLiteral("Endless Highways"), 240000, QStringLiteral("#E91E63"), true);
        addTrack(QStringLiteral("City Lights"), QStringLiteral("Urban Sounds"),
                 QStringLiteral("Metro Vibes"), 180000, QStringLiteral("#9C27B0"), true);
        addTrack(QStringLiteral("Open Road"), QStringLiteral("Freedom Riders"),
                 QStringLiteral("Journey West"), 210000, QStringLiteral("#FF5722"), true);
        addTrack(QStringLiteral("Sunset Drive"), QStringLiteral("Coastal Tunes"),
                 QStringLiteral("Pacific Dreams"), 225000, QStringLiteral("#FF9800"), true);
    } else if (m_currentPlaylist == QStringLiteral("Road Trip")) {
        addTrack(QStringLiteral("Born to Run"), QStringLiteral("Classic Rockers"),
                 QStringLiteral("Highway Legends"), 270000, QStringLiteral("#F44336"), false);
        addTrack(QStringLiteral("Take It Easy"), QStringLiteral("Desert Sounds"),
                 QStringLiteral("Southwest Vibes"), 234000, QStringLiteral("#795548"), false);
        addTrack(QStringLiteral("Life is a Highway"), QStringLiteral("Road Warriors"),
                 QStringLiteral("Asphalt Dreams"), 252000, QStringLiteral("#607D8B"), false);
        addTrack(QStringLiteral("On the Road Again"), QStringLiteral("Country Cruisers"),
                 QStringLiteral("Miles to Go"), 180000, QStringLiteral("#8BC34A"), true);
        addTrack(QStringLiteral("Free Bird"), QStringLiteral("Southern Rock"),
                 QStringLiteral("Flying Free"), 545000, QStringLiteral("#3F51B5"), false);
        addTrack(QStringLiteral("Radar Love"), QStringLiteral("Night Drivers"),
                 QStringLiteral("After Midnight"), 390000, QStringLiteral("#673AB7"), false);
    } else if (m_currentPlaylist == QStringLiteral("Chill Vibes")) {
        addTrack(QStringLiteral("Weightless"), QStringLiteral("Ambient Waves"),
                 QStringLiteral("Relaxation"), 480000, QStringLiteral("#00BCD4"), false);
        addTrack(QStringLiteral("Ocean Breeze"), QStringLiteral("Nature Sounds"),
                 QStringLiteral("Serenity"), 360000, QStringLiteral("#03A9F4"), false);
        addTrack(QStringLiteral("Moonlight Sonata"), QStringLiteral("Classical Piano"),
                 QStringLiteral("Evening Classics"), 420000, QStringLiteral("#2196F3"), true);
        addTrack(QStringLiteral("Starry Night"), QStringLiteral("Lofi Beats"),
                 QStringLiteral("Night Studies"), 240000, QStringLiteral("#3F51B5"), false);
    } else if (m_currentPlaylist == QStringLiteral("Workout")) {
        addTrack(QStringLiteral("Eye of the Tiger"), QStringLiteral("Power Anthems"),
                 QStringLiteral("Champion"), 245000, QStringLiteral("#F44336"), true);
        addTrack(QStringLiteral("Stronger"), QStringLiteral("Electronic Beats"),
                 QStringLiteral("Peak Performance"), 210000, QStringLiteral("#E91E63"), false);
        addTrack(QStringLiteral("Till I Collapse"), QStringLiteral("Hip Hop Motivation"),
                 QStringLiteral("No Limits"), 298000, QStringLiteral("#9C27B0"), false);
        addTrack(QStringLiteral("Pump It"), QStringLiteral("Dance Energy"),
                 QStringLiteral("Cardio Mix"), 212000, QStringLiteral("#FF5722"), false);
        addTrack(QStringLiteral("Can't Hold Us"), QStringLiteral("Modern Hip Hop"),
                 QStringLiteral("Rise Up"), 258000, QStringLiteral("#FF9800"), true);
    } else if (m_currentPlaylist == QStringLiteral("Jazz Classics")) {
        addTrack(QStringLiteral("Take Five"), QStringLiteral("Dave Brubeck"),
                 QStringLiteral("Time Out"), 324000, QStringLiteral("#795548"), true);
        addTrack(QStringLiteral("So What"), QStringLiteral("Miles Davis"),
                 QStringLiteral("Kind of Blue"), 561000, QStringLiteral("#607D8B"), true);
        addTrack(QStringLiteral("My Favorite Things"), QStringLiteral("John Coltrane"),
                 QStringLiteral("My Favorite Things"), 822000, QStringLiteral("#9E9E9E"), false);
        addTrack(QStringLiteral("Autumn Leaves"), QStringLiteral("Bill Evans"),
                 QStringLiteral("Portrait in Jazz"), 326000, QStringLiteral("#FF8F00"), false);
    }

    m_currentTrackIndex = 0;
    emit queueChanged();
}

void MediaController::loadPlaylists()
{
    m_playlists.clear();

    auto addPlaylist = [this](const QString& name, const QString& color, int trackCount) {
        QVariantMap playlist;
        playlist[QStringLiteral("name")] = name;
        playlist[QStringLiteral("color")] = color;
        playlist[QStringLiteral("trackCount")] = trackCount;
        m_playlists.append(playlist);
    };

    addPlaylist(QStringLiteral("Liked Songs"), QStringLiteral("#1DB954"), 5);
    addPlaylist(QStringLiteral("Road Trip"), QStringLiteral("#F44336"), 6);
    addPlaylist(QStringLiteral("Chill Vibes"), QStringLiteral("#00BCD4"), 4);
    addPlaylist(QStringLiteral("Workout"), QStringLiteral("#FF5722"), 5);
    addPlaylist(QStringLiteral("Jazz Classics"), QStringLiteral("#795548"), 4);

    emit playlistsChanged();
}

void MediaController::updateCurrentTrack()
{
    if (m_queue.isEmpty() || m_currentTrackIndex >= m_queue.size()) {
        return;
    }

    QVariantMap track = m_queue.at(m_currentTrackIndex).toMap();
    m_title = track.value(QStringLiteral("title")).toString();
    m_artist = track.value(QStringLiteral("artist")).toString();
    m_album = track.value(QStringLiteral("album")).toString();
    m_duration = track.value(QStringLiteral("duration")).toInt();
    m_albumArtColor = track.value(QStringLiteral("artColor")).toString();
    m_liked = track.value(QStringLiteral("liked")).toBool();

    emit trackChanged();
    emit likedChanged();
}

void MediaController::toggleLiked()
{
    m_liked = !m_liked;

    // Update in queue
    if (m_currentTrackIndex < m_queue.size()) {
        QVariantMap track = m_queue.at(m_currentTrackIndex).toMap();
        track[QStringLiteral("liked")] = m_liked;
        m_queue[m_currentTrackIndex] = track;
    }

    emit likedChanged();
}

void MediaController::setShuffleMode(bool shuffle)
{
    if (m_shuffleMode != shuffle) {
        m_shuffleMode = shuffle;
        emit shuffleModeChanged();
    }
}

void MediaController::cycleRepeatMode()
{
    m_repeatMode = (m_repeatMode + 1) % 3;  // 0 -> 1 -> 2 -> 0
    emit repeatModeChanged();
}

void MediaController::setVolume(int vol)
{
    vol = qBound(0, vol, 100);
    if (m_volume != vol) {
        m_volume = vol;
        emit volumeChanged();
    }
}

void MediaController::selectPlaylist(const QString& playlistName)
{
    if (m_currentPlaylist != playlistName) {
        m_currentPlaylist = playlistName;
        m_position = 0;
        loadMockData();
        updateCurrentTrack();
        emit playlistChanged();
        emit positionChanged(m_position);
    }
}

void MediaController::playTrackAt(int index)
{
    if (index >= 0 && index < m_queue.size()) {
        m_currentTrackIndex = index;
        m_position = 0;
        updateCurrentTrack();
        emit positionChanged(m_position);
        play();
    }
}

} // namespace infotainment
} // namespace automotive
