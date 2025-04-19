// Remove unused Rng import
use libc::c_float;

const FILTER_LENGTH: usize = 128;
const MU: f32 = 0.01;

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

        let error = primary - y;

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