/* make_wav.h
 * Fri Jun 18 17:06:02 PDT 2010 Kevin Karplus
 */

#ifndef MAKE_WAV_H
#define MAKE_WAV_H
#include <QString>

void write_wav(QString filename, double num_samples, QVector<double> data, double s_rate);
    /* open a file named filename, write signed 16-bit values as a
        monoaural WAV file at the specified sampling rate
        and close the file
    */

void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file);

#endif
