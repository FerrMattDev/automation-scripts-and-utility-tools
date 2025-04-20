from __future__ import unicode_literals
import yt_dlp

ydl_opts = {
    'format': 'bestaudio/best',
    'postprocessors': [{
        'key': 'FFmpegExtractAudio',
        'preferredcodec': 'mp3',
        'preferredquality': '192',
    }],
}

while True:
    url = input("Please enter the YouTube URL you want to download audio from: ")

    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        try:
            ydl.download([url])
            print(f"Successfully downloaded audio from: {url}")
        except yt_dlp.DownloadError as e:
            print(f"Error downloading audio from {url}: {e}")
        except Exception as e:
            print(f"An unexpected error occurred: {e}")

    while True:
        another = input("Do you want to convert another song? (y/n): ").lower()
        if another in ['y', 'n']:
            break
        else:
            print("Invalid input. Please enter 'y' for yes or 'n' for no.")

    if another == 'n':
        break

print("Thank you for using the YouTube to MP3 converter!")
