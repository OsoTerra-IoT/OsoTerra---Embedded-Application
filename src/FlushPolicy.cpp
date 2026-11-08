#include "FlushPolicy.h"

FlushPolicy::FlushPolicy(int confirmReadings, unsigned long cooldownMs, int maxCycles, float saturatedPercent)
    : confirmReadings(confirmReadings), cooldownMs(cooldownMs), maxCycles(maxCycles), saturatedPercent(saturatedPercent) {}

FlushPolicy::Decision FlushPolicy::evaluate(float conductivityDsM, float cropThresholdDsM, float moisturePercent,
                                            unsigned long now) {
  float ratio = conductivityDsM / cropThresholdDsM;
  if (ratio < 1.0f) {  // back to Normal: the problem is solved, restart the counters
    criticalStreak = 0;
    cycles = 0;
    return NONE;
  }
  if (ratio < CRITICAL_RATIO) {
    criticalStreak = 0;
    return NONE;
  }
  if (++criticalStreak < confirmReadings) return NONE;  // one noisy reading is not enough
  if (cycles >= maxCycles) return MAX_CYCLES_REACHED;
  if (moisturePercent >= saturatedPercent) return SOIL_SATURATED;
  if (hasFlushed && now - lastFlushEndedAt < cooldownMs) return WAIT_COOLDOWN;
  return START;
}
