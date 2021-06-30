// #define UNICODE
#include <windows.h>
#include <stdio.h>




void WriteWaveData(void)
{


    // HANDLE      hFormat;


    WAVEFORMATEX wfx =
    { WAVE_FORMAT_PCM // WORD  wFormatTag;
    , 1               // WORD  nChannels;
    , 8000            // DWORD nSamplesPerSec;
    , 8000 * 1        // DWORD nAvgBytesPerSec; = nSamplesPerSec * nBlockAlign
    , 1 * 8 / 8       // WORD  nBlockAlign; = nChannels * wBitsPerSample / 8
    , 8               // WORD  wBitsPerSample; should be equal to 8 or 16
    , 0               // WORD  cbSize; ignored
    };


    int seconds = 30;
    DWORD dwDataSize = 8000 * seconds;

     char data[ 30 * 8000 ];
    for ( int i = 0; i < 30 * 8000; ++i ) {
      data[i] = ( i << 2 ) % 255;
    }



    // lpData = ( HPSTR ) data;

    // HMMIO hmmio;

    // MMIOINFO mmioinfo =
      // { MMIO_ALLOCBUF // DWORD      dwFlags;
      // , NULL // FOURCC     fccIOProc;
      // , NULL // LPMMIOPROC pIOProc;
      // , NULL // UINT       wErrorRet;
      // , NULL // HTASK      hTask;
      // , dwDataSize // LONG       cchBuffer;
      // , lpData // HPSTR      pchBuffer;
      // , NULL // HPSTR      pchNext;
      // , NULL // HPSTR      pchEndRead;
      // , NULL // HPSTR      pchEndWrite;
      // , NULL // LONG       lBufOffset;
      // , NULL // LONG       lDiskOffset;
      // , NULL // DWORD      adwInfo[4];
      // , NULL // DWORD      dwReserved1;
      // , NULL // DWORD      dwReserved2;
      // , NULL //hmmio // HMMIO      hmmio;
    // };


    // hmmio =
      // mmioOpen
        // ( NULL          // LPTSTR szFilename, When opening a memory file, set szFilename to NULL
        // , &mmioinfo     // LPMMIOINFO lpmmioinfo, all unused members of MMIOINFO must be set to zero
        // , MMIO_ALLOCBUF // DWORD dwOpenFlags,
    // );


    // MMRESULT mmresult =
      // mmioSetBuffer
        // ( hmmio
        // , NULL // LPSTR pchBuffer,
        // , dwDataSize // LONG  cchBuffer,
        // , 0 // UINT  wFlags, reserved
      // );

    // printf ( "MMRESULT: %d ( zero is ok )\n", mmresult );
    // printf ( "hmmio is NULL ? %s\n", hmmio == NULL ? "yes" : "no" );
    // printf ( "MMIOERR_CANNOTWRITE: %d\n", MMIOERR_CANNOTWRITE );
    // printf ( "MMIOERR_OUTOFMEMORY: %d\n", MMIOERR_OUTOFMEMORY );



    // Open a waveform device for output using window callback.

    HWAVEOUT hWaveOut;

    if ( waveOutOpen
          ( ( LPHWAVEOUT ) &hWaveOut
          , WAVE_MAPPER
          , ( LPWAVEFORMATEX ) &wfx
          , ( DWORD ) NULL // no callback // , (LONG)hwndApp
          , 0L
          , CALLBACK_NULL // CALLBACK_WINDOW
          )
       )
    {
        MessageBox ( NULL,
                   "Failed to open waveform output device.",
                   NULL, MB_OK | MB_ICONEXCLAMATION);
        // LocalUnlock ( hFormat );
        // LocalFree ( hFormat );
        // mmioClose ( hmmio, 0 );
        return;
    }

    // Allocate and lock memory for the waveform data.



// HANDLE hData  = NULL;  // handle of waveform data memory


    // HANDLE hData = GlobalAlloc ( GMEM_MOVEABLE | GMEM_SHARE, dwDataSize );
    // if ( !hData )
    // {
        // MessageBox ( NULL, "Out of memory.",
                   // NULL, MB_OK | MB_ICONEXCLAMATION);
        // // mmioClose(hmmio, 0);
        // return;
    // }

    // HPSTR lpData = GlobalLock ( hData );  // pointer to waveform data memory

    // // &data; //

    // if ( lpData == NULL )
    // {

        // MessageBox ( NULL, "Failed to lock memory for data chunk.",
                   // NULL, MB_OK | MB_ICONEXCLAMATION);
        // GlobalFree(hData);
        // // mmioClose(hmmio, 0);
        // return;
    // }

    // Read the waveform data subchunk.

    // LONG mmioReadstatus = mmioRead ( hmmio, (HPSTR) lpData, dwDataSize);

    // if( mmioReadstatus != ( LRESULT ) dwDataSize )
    // {

      // // DWORD lasterr = GetLastError();
      // printf( "mmioReadstatus: %d\n", mmioReadstatus);



        // MessageBox ( NULL, "Failed to read data chunk.",
                   // NULL, MB_OK | MB_ICONEXCLAMATION);
        // GlobalUnlock(hData);
        // GlobalFree(hData);
        // mmioClose(hmmio, 0);
        // return;
    // }

    // Allocate and lock memory for the header.

    HGLOBAL hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, (DWORD) sizeof(WAVEHDR));
    if (hWaveHdr == NULL)
    {
        // GlobalUnlock(hData);
        // GlobalFree(hData);
        MessageBox ( NULL, "Not enough memory for header.",
            NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }


    LPWAVEHDR lpWaveHdr = ( LPWAVEHDR ) GlobalLock ( hWaveHdr );
    if ( lpWaveHdr == NULL )
    {
        // GlobalUnlock(hData);
        // GlobalFree(hData);
        MessageBox ( NULL,
            "Failed to lock memory for header.",
            NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    // After allocation, set up and prepare header.

    // char *abc = data;
    // for ( int i = 0; i < 10; ++i )
    // printf ( "%d ", abc[i] );

    lpWaveHdr->lpData = data;//lpData;
    lpWaveHdr->dwBufferLength = dwDataSize;
    lpWaveHdr->dwFlags = WHDR_PREPARED;
    lpWaveHdr->dwLoops = 0L;
    waveOutPrepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));

    // Now the data block can be sent to the output device. The
    // waveOutWrite function returns immediately and waveform
    // data is sent to the output device in the background.

    UINT wResult = waveOutWrite(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
    if (wResult != 0)
    {
      printf("wResult : %d\n", wResult);

        waveOutUnprepareHeader(hWaveOut, lpWaveHdr,
                               sizeof(WAVEHDR));
        // GlobalUnlock( hData);
        // GlobalFree(hData);
        MessageBox ( NULL, "Failed to write block to device",
                   NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
}










int
main () {



  WriteWaveData();

  printf ( "hello, audio!\n" );
}