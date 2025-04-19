use noise_cancellation::process_audio_frame;  // Use crate name directly

const TEST_FRAME_SIZE: usize = 256;
const SAMPLE_RATE: f32 = 16000.0;
const DELAY_MS: u32 = 5;
const DELAY_SAMPLES: usize = (DELAY_MS as f32 * SAMPLE_RATE / 1000.0) as usize;

fn generate_sine_wave(freq: f32, time: f32) -> f32 {
    (2.0 * std::f32::consts::PI * freq * time).sin()
}


// Add required Rng trait import
use rand::Rng;


// Fix unused time parameter
fn generate_correlated_noise(_time: f32) -> f32 {  // Add underscore prefix
    let mut rng = rand::thread_rng();
    rng.gen_range(-1.0..1.0)  // Proper range syntax
}

fn main() {
    let mut main_mic = vec![0.0; TEST_FRAME_SIZE];
    let mut ref_mic = vec![0.0; TEST_FRAME_SIZE];
    let mut output = vec![0.0; TEST_FRAME_SIZE];
    let mut ref_noise_buffer = vec![0.0; DELAY_SAMPLES];
    let mut buffer_index = 0;

    for i in 0..TEST_FRAME_SIZE {
        let time = i as f32 / SAMPLE_RATE;
        let current_ref = generate_correlated_noise(time) * 0.5;
        ref_mic[i] = current_ref;

        main_mic[i] = generate_sine_wave(1000.0, time) * 0.8 + 
                      ref_noise_buffer[buffer_index] * 0.2;

        ref_noise_buffer[buffer_index] = current_ref;
        buffer_index = (buffer_index + 1) % DELAY_SAMPLES;
    }

    // Remove unnecessary unsafe block
    process_audio_frame(
        main_mic.as_ptr(),
        ref_mic.as_ptr(),
        output.as_mut_ptr(),
        TEST_FRAME_SIZE as u32,
    );

    let rms = output.iter()
        .fold(0.0f32, |acc, &x| acc + x * x)  // Specify f32 type
        .sqrt() / (TEST_FRAME_SIZE as f32).sqrt();
        
    println!("Noise cancellation test RMS: {:.4}", rms);
}