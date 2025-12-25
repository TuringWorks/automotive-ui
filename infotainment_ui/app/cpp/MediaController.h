// MediaController.h
// Media playback controller (mock)

#ifndef AUTOMOTIVE_MEDIA_CONTROLLER_H
#define AUTOMOTIVE_MEDIA_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>

namespace automotive {
namespace infotainment {

/**
 * @brief Media controller (mock implementation)
 */
class MediaController : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(QString title READ title NOTIFY trackChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY trackChanged)
    Q_PROPERTY(QString album READ album NOTIFY trackChanged)
    Q_PROPERTY(int duration READ duration NOTIFY trackChanged)
    Q_PROPERTY(int position READ position NOTIFY positionChanged)
    Q_PROPERTY(QString source READ source NOTIFY sourceChanged)
    Q_PROPERTY(QVariantList queue READ queue NOTIFY queueChanged)

    // Spotify-like properties
    Q_PROPERTY(QString albumArtColor READ albumArtColor NOTIFY trackChanged)
    Q_PROPERTY(bool liked READ isLiked NOTIFY likedChanged)
    Q_PROPERTY(bool shuffleMode READ shuffleMode NOTIFY shuffleModeChanged)
    Q_PROPERTY(int repeatMode READ repeatMode NOTIFY repeatModeChanged)
    Q_PROPERTY(int volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(QVariantList playlists READ playlists NOTIFY playlistsChanged)
    Q_PROPERTY(QString currentPlaylist READ currentPlaylist NOTIFY playlistChanged)
    Q_PROPERTY(int currentTrackIndex READ currentTrackIndex NOTIFY trackChanged)

public:
    explicit MediaController(QObject* parent = nullptr);
    ~MediaController() override;

    bool isPlaying() const { return m_playing; }
    QString title() const { return m_title; }
    QString artist() const { return m_artist; }
    QString album() const { return m_album; }
    int duration() const { return m_duration; }
    int position() const { return m_position; }
    QString source() const { return m_source; }
    QVariantList queue() const { return m_queue; }

    // Spotify-like getters
    QString albumArtColor() const { return m_albumArtColor; }
    bool isLiked() const { return m_liked; }
    bool shuffleMode() const { return m_shuffleMode; }
    int repeatMode() const { return m_repeatMode; }  // 0=off, 1=all, 2=one
    int volume() const { return m_volume; }
    QVariantList playlists() const { return m_playlists; }
    QString currentPlaylist() const { return m_currentPlaylist; }
    int currentTrackIndex() const { return m_currentTrackIndex; }

public slots:
    void play();
    void pause();
    void togglePlayPause();
    void next();
    void previous();
    void seek(int positionMs);
    void setSource(const QString& source);

    // Spotify-like slots
    void toggleLiked();
    void setShuffleMode(bool shuffle);
    void cycleRepeatMode();
    void setVolume(int vol);
    void selectPlaylist(const QString& playlistName);
    void playTrackAt(int index);

signals:
    void playingChanged(bool playing);
    void trackChanged();
    void positionChanged(int position);
    void sourceChanged(const QString& source);
    void queueChanged();
    void likedChanged();
    void shuffleModeChanged();
    void repeatModeChanged();
    void volumeChanged();
    void playlistsChanged();
    void playlistChanged();

private slots:
    void onPositionTick();

private:
    void loadMockData();
    void loadPlaylists();
    void updateCurrentTrack();

    bool m_playing{false};
    QString m_title{QStringLiteral("Sample Track")};
    QString m_artist{QStringLiteral("Sample Artist")};
    QString m_album{QStringLiteral("Sample Album")};
    int m_duration{240000};  // 4 minutes
    int m_position{0};
    QString m_source{QStringLiteral("Bluetooth")};
    QVariantList m_queue;
    QTimer* m_positionTimer{nullptr};

    // Spotify-like state
    QString m_albumArtColor{QStringLiteral("#1DB954")};
    bool m_liked{false};
    bool m_shuffleMode{false};
    int m_repeatMode{0};  // 0=off, 1=all, 2=one
    int m_volume{80};
    QVariantList m_playlists;
    QString m_currentPlaylist{QStringLiteral("Liked Songs")};
    int m_currentTrackIndex{0};
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_MEDIA_CONTROLLER_H
