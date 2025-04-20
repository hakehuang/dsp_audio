// Remove unused Rng import
use libc::c_float;

const FILTER_LENGTH: usize = 128;
const MU: f32 = 0.01;

// Add noise measurement state
static mut ADC_NOISE_RMS: f32 = 0.0;
static mut NOISE_CALIBRATED: bool = false;

pub fn calibrate_adc_noise(calibration_samples: usize) {
    let mut sum = 0.0;
    
    for _ in 0..calibration_samples {
        let sample = 0.0; // Replace with actual ADC read
        sum += sample * sample;
    }
    
    unsafe {
        ADC_NOISE_RMS = (sum / calibration_samples as f32).sqrt();
        NOISE_CALIBRATED = true;
    }
}

pub struct LMSFilter {
    w: [f32; FILTER_LENGTH],
    x: [f32; FILTER_LENGTH],
    index: usize,
}

impl LMSFilter {
    pub fn new() -> Self {
        LMSFilter {
            w: [0.0; FILTER_LENGTH],
            x: [0.0; FILTER_LENGTH],
            index: 0,
        }
    }

    pub fn process(&mut self, primary: f32, reference: f32) -> f32 {
        self.x[self.index] = reference;
        
        let mut y = 0.0;
        let mut tap = self.index;
        for i in 0..FILTER_LENGTH {
            y += self.w[i] * self.x[tap];
            tap = if tap == 0 { FILTER_LENGTH - 1 } else { tap - 1 };
        }

        let mut error = primary - y;

        // Add noise threshold
        unsafe {
            if NOISE_CALIBRATED && error.abs() < ADC_NOISE_RMS * 3.0 {
                error = 0.0;
            }
        }

        let mut tap = self.index;
        for i in 0..FILTER_LENGTH {
            self.w[i] += MU * error * self.x[tap];
            tap = if tap == 0 { FILTER_LENGTH - 1 } else { tap - 1 };
        }

        self.index = (self.index + 1) % FILTER_LENGTH;
        error
    }
}

#[no_mangle]
pub extern "C" fn process_audio_frame(
    main_mic: *const c_float,
    ref_mic: *const c_float,
    output: *mut c_float,
    frame_size: u32,
) {
    // Keep unsafe contained to pointer operations
    unsafe {
        let main_slice = std::slice::from_raw_parts(main_mic, frame_size as usize);
        let ref_slice = std::slice::from_raw_parts(ref_mic, frame_size as usize);
        let out_slice = std::slice::from_raw_parts_mut(output, frame_size as usize);
        
        let mut filter = LMSFilter::new();
        for i in 0..frame_size as usize {
            out_slice[i] = filter.process(main_slice[i], ref_slice[i]);
        }
    }
}

#[no_mangle]
pub extern "C" fn calibrate_adc_noise_ffi(calibration_samples: u32) {
    calibrate_adc_noise(calibration_samples as usize);
}