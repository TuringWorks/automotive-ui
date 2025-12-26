# ADAS UI Usage Scenarios

## Overview

This document describes key usage scenarios for the ADAS UI system, covering both nominal operation and edge cases for safety and security validation.

## Nominal Scenarios

### SC-001: L2 Highway Engagement

**Preconditions:**
- Vehicle on highway
- Speed > 40 mph
- Lane markings visible
- Clear weather

**Flow:**
1. Driver activates ACC via steering wheel control
2. AdasStateService receives engagement command
3. System validates conditions (available=true)
4. State transitions: Off → Available → EngagedL2
5. AdasStateZone displays "ENGAGED - L2 PARTIAL AUTOMATION"
6. Feature badges show ACC ✓, LKA ✓
7. Set speed and following gap displayed

**Expected UI State:**
```
┌─────────────────────────────────────────┐
│ [L2] ENGAGED - PARTIAL AUTOMATION       │
│ Driver in control                       │
│                                         │
│ [ACC ✓] [LKA ✓]    Set: 75 mph         │
│                     Gap: 2.0 sec        │
└─────────────────────────────────────────┘
```

**Timing:**
- Engagement display: < 100ms from signal receipt
- Feature badge update: < 100ms

---

### SC-002: Lead Vehicle Following

**Preconditions:**
- L2 engaged
- Lead vehicle detected at 60m

**Flow:**
1. PerceptionModel receives lead vehicle data
2. Lead vehicle marker appears in PerceptionMiniView
3. Time gap calculation displayed
4. ACC adjusts to maintain gap

**Expected UI State:**
```
┌─────────────────┐
│   ▼ (40m)       │
│   │ 2.1 sec     │
│  ─┼─            │
│  │ │            │
└─────────────────┘
```

---

### SC-003: Lane Quality Degradation

**Preconditions:**
- L2 engaged
- Lane markings becoming faint

**Flow:**
1. PerceptionModel receives low-confidence lane data
2. Lane boundary colors change (green → yellow → orange)
3. If confidence drops below threshold:
   - Perception degraded signal emitted
   - "DEGRADED" state displayed within 300ms
4. Lane visualization shows dashed lines for low confidence

**Expected UI State:**
```
Lane Quality: ████░░░░░░ 40%
[!] Perception degraded - lane lines unclear
```

**Timing:**
- DEGRADED indication: < 300ms (SR-CL-ADAS-140)

---

### SC-004: Takeover Request (ODD Exit)

**Preconditions:**
- L3 engaged (conditional automation)
- Approaching ODD boundary

**Flow:**
1. ODD monitor detects approaching boundary
2. TakeoverManager receives Advisory urgency request
3. PreWarning state entered
4. TakeoverBanner appears with 30-second countdown
5. If no acknowledgment:
   - Escalate to Warning (10 sec)
   - Escalate to Immediate (5 sec)
   - Escalate to Critical (3 sec)
   - MRM initiated

**TakeoverBanner States:**
```
Advisory (30 sec):
┌─────────────────────────────────────────────────────┐
│ ⚠ PREPARE TO TAKE OVER              [30] sec       │
│ Approaching highway exit                            │
└─────────────────────────────────────────────────────┘

Warning (10 sec):
┌─────────────────────────────────────────────────────┐
│ ⚠ TAKE OVER NOW                     [10] sec       │
│ ODD boundary ahead                                  │
└─────────────────────────────────────────────────────┘ (pulsing)

Immediate (5 sec):
┌─────────────────────────────────────────────────────┐
│ ⚠ IMMEDIATE ACTION REQUIRED         [5] sec        │
│ Take control of the vehicle                         │
└─────────────────────────────────────────────────────┘ (flashing)

Critical (3 sec):
┌─────────────────────────────────────────────────────┐
│ ⚠ CRITICAL - TAKE OVER NOW          [3] sec        │
│ MRM will activate if no response                    │
└─────────────────────────────────────────────────────┘ (rapid flash)
```

**Timing:**
- Banner appearance: < 100ms (SR-CL-ADAS-120)

---

### SC-005: Minimal Risk Maneuver (MRM)

**Preconditions:**
- Takeover request active
- Driver fails to respond

**Flow:**
1. Countdown expires
2. TakeoverManager transitions to MrmInitiated
3. TakeoverBanner shows MRM state
4. Vehicle begins controlled stop

**Expected UI State:**
```
┌─────────────────────────────────────────────────────┐
│ ⚠ SAFE STOP IN PROGRESS                            │
│ Vehicle is stopping safely                          │
└─────────────────────────────────────────────────────┘ (red, steady)
```

---

## Edge Case Scenarios

### SC-010: Replay Attack Detection

**Preconditions:**
- System receiving ADAS messages
- Attacker replays previous message

**Flow:**
1. AdasStateService receives message with old sequence number
2. Sequence number < last valid sequence number
3. Message rejected
4. integrityFailure signal emitted
5. Event logged with CR-ADAS-102 tag
6. Last valid state maintained (CR-ADAS-103)

**Logging:**
```
[SECURITY] Message rejected - replay attack detected
           Expected seq: >100, Received: 50
```

---

### SC-011: Stale Perception Data

**Preconditions:**
- L2 engaged
- Perception data stops arriving

**Flow:**
1. Last perception update > 200ms ago
2. processTick() detects staleness
3. Lane model cleared
4. Object list cleared
5. DEGRADED state displayed within 300ms

**Timing:**
- Staleness detection: 200ms window
- DEGRADED display: < 300ms (SR-CL-ADAS-142)

---

### SC-012: Conflicting Signals

**Preconditions:**
- System receiving engagement data
- engaged=true but available=false

**Flow:**
1. AdasStateService receives conflicting data
2. Safety logic: engaged AND available required
3. isEngaged() returns false
4. Display shows NOT engaged (SR-CL-ADAS-100)

---

### SC-013: Engineering Mode Motion Lock

**Preconditions:**
- User in Autonomy Center
- Attempting to access Engineering tab
- Vehicle speed > 5 mph

**Flow:**
1. User taps Engineering tab
2. PerceptionExplorer checks vehicle speed
3. Speed > 5 mph detected
4. Motion lock overlay displayed
5. Engineering controls disabled

**Expected UI State:**
```
┌─────────────────────────────────────────────────────┐
│            ⚠ Vehicle in Motion                      │
│                                                      │
│  Engineering mode requires vehicle to be stopped     │
│  (speed < 5 mph)                                     │
│                                                      │
│  Current speed: 35 mph                               │
└─────────────────────────────────────────────────────┘
```

---

### SC-014: Weather Condition Change

**Preconditions:**
- L2 engaged
- Clear weather

**Flow:**
1. Weather detection: rain starting
2. PerceptionModel updates weather condition
3. WeatherParticles activates rain effect
4. If heavy rain:
   - ODD constraint violated
   - System transitions to degraded or unavailable

**3D Scene Change:**
- Rain particles appear progressively
- Road surface shows wet reflections (if quality allows)
- Lane boundary confidence may decrease

---

### SC-015: Multiple Object Tracking

**Preconditions:**
- L2 engaged on highway
- Multiple vehicles in perception range

**Flow:**
1. ObjectList receives multiple objects
2. ObjectInstances updates instanced rendering
3. Lead vehicle highlighted
4. Relevant objects marked

**Expected Visualization:**
- Lead vehicle: Blue marker with distance
- Adjacent vehicles: Grey markers
- Pedestrians: Yellow markers (if detected)
- Cyclists: Orange markers (if detected)

---

## Autonomy Center Scenarios

### SC-020: Viewing Feature Limitations

**Flow:**
1. User opens Autonomy Center
2. Navigates to Overview tab
3. Taps "Why unavailable?" for LCA
4. WhyUnavailable shows limitation list

**Expected Content:**
```
Why is Lane Change Assist unavailable?

⚠ Speed below minimum (40 mph required)
  Current: 25 mph

✓ Lane markings detected
✓ Camera system operational
✓ Radar system operational
```

---

### SC-021: Event History Review

**Flow:**
1. User opens Autonomy Center
2. Navigates to Events tab
3. Filters by "Takeover" category
4. Reviews past takeover requests

**Expected Content:**
```
Takeover Events - Today

14:32 [WARN] Takeover request - ODD ending
      Duration: 8.2 seconds
      Result: Driver acknowledged

11:15 [WARN] Takeover request - Weather
      Duration: 5.1 seconds
      Result: Driver acknowledged
```

---

### SC-022: Settings Modification

**Flow:**
1. User opens Autonomy Center
2. Navigates to Settings tab
3. Disables LCA feature
4. Adjusts DMS sensitivity to High

**Expected Behavior:**
- Settings persisted immediately
- LCA no longer activates even when conditions allow
- DMS alerts trigger at lower distraction threshold

---

## Safety Verification Scenarios

### SV-001: 300ms Unavailable Timing

**Test Procedure:**
1. Engage L2 system
2. Inject unavailable signal
3. Measure time to UI update

**Acceptance Criteria:**
- UI shows "Unavailable" within 300ms (SR-CL-ADAS-101)

---

### SV-002: Non-Occludable Takeover

**Test Procedure:**
1. Trigger takeover request
2. Attempt to navigate away
3. Attempt to dismiss banner

**Acceptance Criteria:**
- Banner remains visible at all times (SR-CL-ADAS-120)
- z-index verified as 1000
- No dismiss action available

---

### SV-003: MRM Display Persistence

**Test Procedure:**
1. Trigger MRM state
2. Verify message content
3. Verify display persistence

**Acceptance Criteria:**
- "SAFE STOP" message displayed (SR-CL-ADAS-130)
- Display remains until MRM complete
- No user dismissal possible

---

## Cybersecurity Verification Scenarios

### CV-001: Sequence Number Validation

**Test Procedure:**
1. Send valid message with seq=100
2. Send message with seq=50 (replay)
3. Verify rejection

**Acceptance Criteria:**
- Second message rejected (CR-ADAS-102)
- integrityFailure signal emitted
- State unchanged

---

### CV-002: Tamper-Evident Log Verification

**Test Procedure:**
1. Log multiple events
2. Attempt to modify middle event
3. Verify checksum chain broken

**Acceptance Criteria:**
- Checksum verification fails (CR-INF-ADAS-010)
- Tampering detected and logged

---

## Related Documentation

- [ADAS UI Overview](overview.md)
- [Driver UI Requirements](driver_ui_requirements.md)
- [Infotainment Requirements](infotainment_requirements.md)
- [Security Architecture](security.md)
