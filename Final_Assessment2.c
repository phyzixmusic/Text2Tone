/*

 MUT-30004

 Mohamed Eid - 13010389
  
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <portsf.h>
#include <math.h> 

//Arguments
enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_SAMPRATE, ARG_ENVWARP, ARG_TRANSP, ARG_NOTEMULT, ARG_FILETYPE, ARGC};

// Functions
long make_sine(float *buffer, float amplitude, long num_frames, double frequency, double samplePeriod);
long attack(float *buffer, long attack_frames, float envWarp);
long decay(float *endBuf, long decay_frames, float envWarp);
int open_output(char* name, PSF_PROPS *pProps);
float* allocate_buffer(long bytes);
long write(int outfile, float* buffer, long num_frames);
void clean_up(FILE* inFP, int outfile, float* buffer);



///* Main Start *///
int main(int argc, char *argv[])
{
    ///* Variables Start *///
    PSF_PROPS props;
    FILE* inFP;

    float MIDI_note;		/*To store data from text file*/
    float MIDI_vel;
    double duration, attackSec, decaySec;

    double frequency;		/*To store audio data*/
    float amplitude;
    long num_frames;
    long bytes;

    int outfile;		/*For output file and buffer*/
    float* buffer;
    float *endBuf;

    long sampleRate;		/*For sample rate and period*/
    double samplePeriod;

    float envWarp;		/*To store envelope warp, transposition and note multiplier data*/
    long transp;
    float noteMult;
    long fileType;

    long attack_frames;		/*To store duration data for attack/decay*/
    long decay_frames;
    ///* Variables End *///

    ///* Argument Check Start *///
    switch(argc)
    {
        case ARGC :
            printf("All arguments supplied. Program %s will now convert file %s\n", argv[ARG_NAME], argv[ARG_INFILE]);
            sampleRate = atol(argv[ARG_SAMPRATE]);
            envWarp = atof(argv[ARG_ENVWARP]);
            transp = atol(argv[ARG_TRANSP]);
            noteMult = atof(argv[ARG_NOTEMULT]);
            fileType = atol(argv[ARG_FILETYPE]);
            break;

        case ARGC -1 :
            printf("File type not specified, set default. Program %s will now convert file %s\n", argv[ARG_NAME], argv[ARG_INFILE]);
            sampleRate = atol(argv[ARG_SAMPRATE]);
            envWarp = atof(argv[ARG_ENVWARP]);
            transp = atol(argv[ARG_TRANSP]);
            noteMult = atof(argv[ARG_NOTEMULT]);
            fileType = 0;
            break;

        case ARGC -2 :
            printf("No note multiplier argument, set default.  Program %s will now convert file %s\n", argv[ARG_NAME], argv[ARG_INFILE]);
            sampleRate = atol(argv[ARG_SAMPRATE]);
            envWarp = atof(argv[ARG_ENVWARP]);
            transp = atol(argv[ARG_TRANSP]);
            noteMult = 1;
            fileType = 0;
            break;

        case ARGC -3 :
            printf("No note multiplier or transposition arguments, set default.  Program %s will now convert file %s\n", argv[ARG_NAME], argv[ARG_INFILE]);
            sampleRate = atol(argv[ARG_SAMPRATE]);
            envWarp = atof(argv[ARG_ENVWARP]);
            transp = 0;
            noteMult = 1;
            fileType = 0;
            break;

        case ARGC -4 :
            printf("No note multiplier, transposition or envelope warp arguments, set default.  Program %s will now convert file %s\n", argv[ARG_NAME], argv[ARG_INFILE]);
            sampleRate = atol(argv[ARG_SAMPRATE]);
            envWarp = 1;
            transp = 0;
            noteMult = 1;
            fileType = 0;
            break;

        case ARGC -5 :
            printf("No note multiplier, transposition, envelope warp or sample rate arguments, set default.  Program %s will now convert file %s\n", argv[ARG_NAME], argv[ARG_INFILE]);
            sampleRate = 44100;
            envWarp = 1;
            transp = 0;
            noteMult = 1;
            fileType = 0;
            break;

        default :
            printf("ERROR: No input or output file specified. Please specify correct file and arguments.\n");
            return -1;
    }
    ///* Argument Checking *///

    ///* Invalid Variable Check Start *///
    if(sampleRate <= 0)
    {	printf("Error: Sample rate cannot be below 0");
        return -1;
    }
    if(envWarp <= 0)
    {	printf("Error: Envelope Warp cannot be below 0");
        return -1;
    }
    if(noteMult <= 0)
    {	printf("ERROR: Note Multiplier cannot be below 0");
        return -1;
    }
    ///* Invalid Variable Check End *///

    /* Start portsf */
    if (psf_init())
    {
        printf("Error: unable to open portsf\n");
        return -2;
    }

    /* Open input file */
    inFP = fopen(argv[ARG_INFILE], "r");
    if(inFP == NULL)
    {
        printf("Error: File not found\n");
        return 1;
    }


    /* Filling in PSF_PROPS structure */
    props.srate = sampleRate;
    props.samptype = PSF_SAMP_16;
    props.chans = 2;	// 2 for stereo
    props.chformat = MC_STEREO; // MC_STEREO
    switch(fileType)
    {
        case 0 :
            printf("Converting to .WAV format...\n");
            props.format = PSF_WAVE_EX;
            break;

        case 1 :
            printf("Converting to .AIFF format...\n");
            props.format = PSF_AIFF;
            break;

        default:
            printf("Not supplied with valid format code. Please select one: .WAV-0, .AIFF-1.");
            return -1;
    }

    /* Create output audio file */
    outfile = open_output(argv[ARG_OUTFILE], &props);
    if(outfile < 0)
        return -10;
    printf("Opened output\n");

    /* Read input data, create waveform and write to output file */
    while (fscanf(inFP, "%f %f %lf %lf %lf", &MIDI_note, &MIDI_vel, &duration, &attackSec, &decaySec ) == 5)
    {
        /* Converting input data to audio data, transposing and multiplying note duration */
        MIDI_note += transp;
        frequency = 440 * pow(2, (MIDI_note-69)/12);
        amplitude = MIDI_vel/127;
        duration *= noteMult;
        samplePeriod = 1.0/sampleRate;
        /* Calculate number of bytes and allocate buffer */
        num_frames = duration * sampleRate;
        bytes = num_frames * props.chans * sizeof(float);
        buffer = allocate_buffer(bytes);
        if(buffer == NULL)
        {
            clean_up(inFP, outfile, buffer);
            return -15;
        }

        /* Generate sine waveform using calculated audio data */
        make_sine(buffer, amplitude, num_frames, frequency, samplePeriod);

        printf("Made Sine\n");
        /* Add envelope to wave */
        attack_frames = attackSec * sampleRate;
        decay_frames = decaySec * sampleRate;
        endBuf = buffer + (num_frames - decay_frames);


        attack(buffer, attack_frames, envWarp);
        printf("Attack\n");
        decay(endBuf, decay_frames, envWarp);
        printf("Decay\n");

        /* Write wave with envelope to output file */
        write(outfile, buffer, num_frames);
        printf("Written outfile\n");

        /* Free buffer */
        free(buffer);
    }

    /* Clean up and Finish portsf */
    clean_up(inFP, outfile, NULL);
    psf_finish();

    /* send completion message */
    switch(fileType)
    {
        case 0 :
            printf("Completed conversion to .WAV\n");
            break;

        case 1:
            printf("Completed conversion to .AIFF\n");
            break;
    }
    printf("Happy Oscilatting");
    return 0;
}
///* Main End *///

///* Function Methods Start *///
/*  Open Output Method */
int open_output(char* name, PSF_PROPS *pProps)
{
    int outfile;

    outfile = psf_sndCreate(name, pProps, 0, 0, PSF_CREATE_RDWR);
    if (outfile < 0)
    {
        printf("cannot create %s\n", name);
    }

    return outfile;
}

/* Allocate Buffer */
float* allocate_buffer(long bytes)
{
    float *buffer;

    buffer = (float*) malloc(bytes);
    if(buffer == 0)
    {
        printf("unable to allocate buffer\n");
    }

    return buffer;
}

/* Write Method */
long write(int outfile, float* buffer, long num_frames)
{
    long frames;


    /* Write the output file */
    frames = psf_sndWriteFloatFrames(outfile, buffer, num_frames);
    if(frames != num_frames)
    {
        printf("Error writing to output\n");
        return -1;
    }

    return frames;
}

/* Clean Up Method */
void clean_up(FILE* inFP, int outfile, float* buffer)
{
    /* Close files */
    if (inFP >= 0)
    {
        fclose(inFP);
    }

    if (outfile >= 0)
    {
        if (psf_sndClose(outfile))
        {
            printf("Warning: error closing output\n");
        }
    }

    /* Free the buffer */
    if(buffer)
    {
        free(buffer);
    }
}

/* Create Sine Method */
long make_sine(float *buffer, float amplitude, long num_frames, double frequency, double samplePeriod)
{
    long i;
    double time;
    double twoPi = 2*M_PI;

    for(i = 0, time = 0 ; i < num_frames ; i++)
    {
        buffer[2*i] = amplitude*sin(twoPi * frequency * time);
        buffer[2*i+1] = amplitude*sin(twoPi * frequency * time);
        time += samplePeriod;
    }
    return i;
}

/* Envelope Attack Method */
long attack(float *buffer, long attack_frames, float envWarp)
{
    long i;

    double factor = 0.0;
    double increment = 1.0/attack_frames;

    for(i = 0; i < attack_frames ; i++)
    {
        buffer[i] = pow(factor, envWarp) * buffer[i];
        factor += increment;
    }
    return i;
}

/* Envelope Decay Method */
long decay(float *endBuf, long decay_frames, float envWarp)
{
    long i;

    double factor = 1.0;
    double decrement = 1.0/decay_frames;

    for(i = 0; i < decay_frames ; i++)
    {
        endBuf[i] = pow(factor, envWarp) * endBuf[i];
        factor -= decrement;
    }
    return i;
}
///* Function Methods End *///

