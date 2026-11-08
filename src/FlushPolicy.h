#ifndef FLUSH_POLICY_H
#define FLUSH_POLICY_H

// Decides when the contingency (washing the soil with fresh water) may start. Pure logic, no hardware.
// Rules: Critical level (EC >= CRITICAL_RATIO x crop threshold) confirmed by several readings in a row, soil not
// saturated, a cooldown between washes so the water drains and the next reading is real, and a maximum of washes
// before asking for a person. The count restarts when the salinity gets back to Normal.
class FlushPolicy {
  int confirmReadings;
  unsigned long cooldownMs;
  int maxCycles;
  float saturatedPercent;
  int criticalStreak = 0;
  int cycles = 0;
  bool hasFlushed = false;
  unsigned long lastFlushEndedAt = 0;

public:
  enum Decision { NONE, START, WAIT_COOLDOWN, MAX_CYCLES_REACHED, SOIL_SATURATED };
  static constexpr float CRITICAL_RATIO = 1.5f;  // same cut as the platform's SalinityAlert CRITICAL

  FlushPolicy(int confirmReadings, unsigned long cooldownMs, int maxCycles, float saturatedPercent);
  Decision evaluate(float conductivityDsM, float cropThresholdDsM, float moisturePercent, unsigned long now);
  void flushStarted() { cycles++; }
  void flushFinished(unsigned long now) { hasFlushed = true; lastFlushEndedAt = now; }
  int getCycles() const { return cycles; }
  int getMaxCycles() const { return maxCycles; }
};

#endif
