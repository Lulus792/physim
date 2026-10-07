#include "builtins.h"
#include <string.h>
#define F PS_TYPE_FLOAT64
#define I PS_TYPE_INT64
#define S PS_TYPE_STRING
#define U PS_TYPE_UNIT
#define QUANTITY PS_TYPE_QUANTITY
#define MEDIUM PS_TYPE_MEDIUM
#define MATERIAL PS_TYPE_MATERIAL
#define DIAGNOSTIC PS_TYPE_DIAGNOSTIC
#define COLLIDER PS_TYPE_COLLIDER
#define WORLD PS_TYPE_CONTACT_WORLD
#define BATCH PS_TYPE_BATCH
#define B PS_TYPE_BOOL
#define RUN_INDEX PS_TYPE_RUN_INDEX
#define RUN_BLOCK PS_TYPE_RUN_BLOCK
#define RUN_SNAPSHOT PS_TYPE_RUN_SNAPSHOT
#define SUBMERSION PS_TYPE_SUBMERSION
#define C PS_TYPE_CHANNEL
#define V2 PS_TYPE_VEC2
#define V3 PS_TYPE_VEC3
#define V4 PS_TYPE_VEC4
#define Q PS_TYPE_QUAT
#define M3 PS_TYPE_MAT3
#define M4 PS_TYPE_MAT4
#define B3 PS_TYPE_BEZIER3
#define VOID PS_TYPE_VOID
#define D PS_TYPE_DATASET
#define R PS_TYPE_SERIES
#define P PS_TYPE_PLOT
#define TABLE PS_TYPE_TABLE
#define DIST PS_TYPE_DISTRIBUTION
#define CONFIG PS_TYPE_SENSOR_CONFIG
#define SENSOR PS_TYPE_SENSOR
#define SAMPLE PS_TYPE_MEASUREMENT
#define RNG PS_TYPE_RNG
#define ODE_RESULT PS_TYPE_ODE_RESULT
#define STEP_INTERVAL PS_TYPE_STEP_INTERVAL
#define SCALAR_RESULT PS_TYPE_SCALAR_RESULT
#define BODY PS_TYPE_BODY
#define CONTACTS PS_TYPE_CONTACTS
#define SOLVER PS_TYPE_CONTACT_SOLVER
#define RESULT PS_TYPE_CONTACT_RESULT
#define JOINT PS_TYPE_DISTANCE_JOINT
#define JOINT_RESULT PS_TYPE_JOINT_RESULT
#define CONTACT_CONSTRAINT PS_TYPE_CONTACT_CONSTRAINT
#define JOINT_CONSTRAINT PS_TYPE_JOINT_CONSTRAINT
#define GRAPH_RESULT PS_TYPE_CONSTRAINT_RESULT
#define SWEEP PS_TYPE_SWEEP
#define AABB PS_TYPE_AABB
static const ps_lang_builtin library[] = {
    {"idealGasPressure", "psrt_gas_pressure", F, 3, 0, {F,F,F}, {"amount","temperature","volume"}},
    {"idealGasVolume", "psrt_gas_volume", F, 3, 0, {F,F,F}, {"amount","temperature","pressure"}},
    {"idealGasTemperature", "psrt_gas_temperature", F, 3, 0, {F,F,F}, {"amount","pressure","volume"}},
    {"idealGasEnergy", "psrt_gas_energy", F, 3, 0, {F,F,F}, {"amount","molarCv","temperature"}},
    {"idealGasEntropyChange", "psrt_gas_entropy", F, 6, 0, {F,F,F,F,F,F}, {"amount","molarCv","initialTemperature","initialVolume","finalTemperature","finalVolume"}},
    {"heatCapacity", "psrt_heat_capacity", F, 2, 0, {F,F}, {"mass","specificHeat"}},
    {"sensibleHeat", "psrt_sensible_heat", F, 3, 0, {F,F,F}, {"capacity","initialTemperature","finalTemperature"}},
    {"heatFlow", "psrt_heat_flow", F, 3, 0, {F,F,F}, {"conductance","temperatureA","temperatureB"}},
    {"thermalReservoirStep", "psrt_thermal_reservoir", F, 5, 0, {F,F,F,F,F}, {"capacity","temperature","reservoirTemperature","conductance","dt"}},
    {"thermalPairStep", "psrt_thermal_pair", V2, 6, 0, {F,F,F,F,F,F}, {"capacityA","temperatureA","capacityB","temperatureB","conductance","dt"}},
    {"Batch", "psrt_batch_make", BATCH, 8, 0, {S,S,S,I,I,F,I,I}, {"module","directory","channel","runs","steps","dt","seed","workers"}},
    {"batchParameter", "psrt_batch_parameter", BATCH, 3, 0, {BATCH,S,F}, {"batch","name","value"}},
    {"batchSweep", "psrt_batch_sweep", BATCH, 4, 0, {BATCH,S,F,F}, {"batch","name","start","end"}},
    {"batchTarget", "psrt_batch_target", BATCH, 2, 0, {BATCH,F}, {"batch","endTime"}},
    {"batchAdaptive", "psrt_batch_adaptive", BATCH, 3, 0, {BATCH,F,F}, {"batch","minimumDt","maximumDt"}},
    {"batchLimits", "psrt_batch_limits", BATCH, 3, 0, {BATCH,F,I}, {"batch","timeout","memoryMiB"}},
    {"batchSource", "psrt_batch_source", BATCH, 2, 0, {BATCH,S}, {"batch","path"}},
    {"batchRun", "psra_batch_run", BATCH, 1, 2, {BATCH}, {"batch"}},
    {"batchRunUntil", "psra_batch_run_until", BATCH, 2, 2, {BATCH,I}, {"batch","completions"}},
    {"batchResume", "psra_batch_resume", BATCH, 2, 2, {S,S}, {"series","directory"}},
    {"batchRequireSuccess", "psra_batch_require_success", VOID, 1, 2, {BATCH}, {"batch"}},
    {"batchSeries", "psra_batch_series", R, 1, 2, {BATCH}, {"batch"}},
    {"outputPrefix", "psra_output_prefix", S, 0, 2, {0}, {NULL}},
    {"batch_directory", "psrt_batch_directory", S, 1, 0, {BATCH}, {"batch"}},
    {"batch_error", "psrt_batch_error", S, 1, 0, {BATCH}, {"batch"}},
    {"batch_module", "psrt_batch_module", S, 1, 0, {BATCH}, {"batch"}},
    {"batch_runs", "psrt_batch_runs", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_steps", "psrt_batch_steps", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_workers", "psrt_batch_workers", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_seed", "psrt_batch_seed", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_completed", "psrt_batch_completed", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_started", "psrt_batch_started", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_reused", "psrt_batch_reused", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_valid", "psrt_batch_valid", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_peakActive", "psrt_batch_peak", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_code", "psrt_batch_code", I, 1, 0, {BATCH}, {"batch"}},
    {"batch_dt", "psrt_batch_dt", F, 1, 0, {BATCH}, {"batch"}},
    {"batch_endTime", "psrt_batch_end_time", F, 1, 0, {BATCH}, {"batch"}},
    {"batch_executed", "psrt_batch_executed", B, 1, 0, {BATCH}, {"batch"}},
    {"batch_cancelled", "psrt_batch_cancelled", B, 1, 0, {BATCH}, {"batch"}},
    {"batch_unit", "psrt_batch_unit", U, 1, 0, {BATCH}, {"batch"}},
    {"batch_values", "psrt_batch_values", PS_LANG_FLOAT_ARRAY, 1, 0, {BATCH}, {"batch"}},
    {"batch_statuses", "psrt_batch_statuses", PS_LANG_INT_ARRAY, 1, 0, {BATCH}, {"batch"}},
    {"batch_value", "psrt_batch_value", F, 2, 0, {BATCH,I}, {"batch","index"}},
    {"batch_status", "psrt_batch_status", I, 2, 0, {BATCH,I}, {"batch","index"}},
    {"batch_finished", "psrt_batch_finished", B, 2, 0, {BATCH,I}, {"batch","index"}},
    {"batch_runPath", "psrt_batch_run_path", S, 2, 0, {BATCH,I}, {"batch","index"}},
    {"ContactWorld", "psrt_world_make", WORLD, 4, 0, {F,F,F,F}, {"matchDistance","minimumNormalDot","maximumDtRatio","warmFraction"}},
    {"defaultContactWorld", "psrt_world_defaults", WORLD, 0, 0, {0}, {NULL}},
    {"worldReset", "psrt_world_reset", WORLD, 1, 0, {WORLD}, {"world"}},
    {"worldSolve", "psrt_world_solve", WORLD, 5, 0, {WORLD,PS_LANG_BODY_ARRAY,PS_LANG_COLLIDER_ARRAY,SOLVER,F}, {"world","bodies","colliders","solver","dt"}},
    {"sphereCollider", "psrt_collider_sphere", COLLIDER, 3, 0, {I,I,F}, {"id","body","radius"}},
    {"boxCollider", "psrt_collider_box", COLLIDER, 3, 0, {I,I,V3}, {"id","body","size"}},
    {"planeCollider", "psrt_collider_plane", COLLIDER, 3, 0, {I,I,V3}, {"id","body","normal"}},
    {"collider_id", "psrt_collider_id", I, 1, 0, {COLLIDER}, {"collider"}},
    {"collider_body", "psrt_collider_body", I, 1, 0, {COLLIDER}, {"collider"}},
    {"collider_shape", "psrt_collider_shape", I, 1, 0, {COLLIDER}, {"collider"}},
    {"collider_size", "psrt_collider_size", V3, 1, 0, {COLLIDER}, {"collider"}},
    {"collider_normal", "psrt_collider_normal", V3, 1, 0, {COLLIDER}, {"collider"}},
    {"world_bodyCount", "psrt_world_body_count", I, 1, 0, {WORLD}, {"world"}},
    {"world_colliderCount", "psrt_world_collider_count", I, 1, 0, {WORLD}, {"world"}},
    {"world_contactCount", "psrt_world_contact_count", I, 1, 0, {WORLD}, {"world"}},
    {"world_matched", "psrt_world_matched", I, 1, 0, {WORLD}, {"world"}},
    {"world_created", "psrt_world_created", I, 1, 0, {WORLD}, {"world"}},
    {"world_ended", "psrt_world_ended", I, 1, 0, {WORLD}, {"world"}},
    {"world_warmed", "psrt_world_warmed", I, 1, 0, {WORLD}, {"world"}},
    {"world_maxNormalError", "psrt_world_normal_error", F, 1, 0, {WORLD}, {"world"}},
    {"world_maxProjectionError", "psrt_world_projection_error", F, 1, 0, {WORLD}, {"world"}},
    {"world_dt", "psrt_world_dt", F, 1, 0, {WORLD}, {"world"}},
    {"world_body", "psrt_world_body", BODY, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_contactIdA", "psrt_world_id_a", I, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_contactIdB", "psrt_world_id_b", I, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_contactPoint", "psrt_world_point", V3, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_contactNormal", "psrt_world_normal", V3, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_localAnchorA", "psrt_world_local_a", V3, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_localAnchorB", "psrt_world_local_b", V3, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_contactImpulse", "psrt_world_impulse", V3, 2, 0, {WORLD,I}, {"world","index"}},
    {"world_contactPenetration", "psrt_world_penetration", F, 2, 0, {WORLD,I}, {"world","index"}},
    {"worldBodies", "psrt_world_bodies", PS_LANG_BODY_ARRAY, 1, 0, {WORLD}, {"world"}},
    {"RunIndex", "psrt_run_index_open", RUN_INDEX, 2, 0, {S,I}, {"path","maximumEntries"}},
    {"runIndexClose", "psrt_run_index_close", VOID, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_is_open", "psrt_run_index_is_open", PS_TYPE_BOOL, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_samples", "psrt_run_index_samples", I, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_snapshots", "psrt_run_index_snapshots", I, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_checkpoints", "psrt_run_index_checkpoints", I, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_channels", "psrt_run_index_channels", I, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_complete", "psrt_run_index_complete", PS_TYPE_BOOL, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_persisted", "psrt_run_index_persisted", PS_TYPE_BOOL, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_metadata", "psrt_run_index_metadata", S, 1, 0, {RUN_INDEX}, {"run"}},
    {"runIndex_name", "psrt_run_index_name", S, 2, 0, {RUN_INDEX,I}, {"run","channel"}},
    {"runIndex_symbol", "psrt_run_index_symbol", S, 2, 0, {RUN_INDEX,I}, {"run","channel"}},
    {"runIndex_description", "psrt_run_index_description", S, 2, 0, {RUN_INDEX,I}, {"run","channel"}},
    {"runIndexDimension", "psrt_run_index_dimension", I, 3, 0, {RUN_INDEX,I,I}, {"run","channel","axis"}},
    {"runIndexRead", "psrt_run_index_read", RUN_BLOCK, 3, 0, {RUN_INDEX,I,I}, {"run","first","count"}},
    {"runIndexSnapshot", "psrt_run_index_snapshot", RUN_SNAPSHOT, 2, 0, {RUN_INDEX,I}, {"run","ordinal"}},
    {"runBlock_count", "psrt_run_block_count", I, 1, 0, {RUN_BLOCK}, {"block"}},
    {"runBlock_channels", "psrt_run_block_channels", I, 1, 0, {RUN_BLOCK}, {"block"}},
    {"runBlock_time", "psrt_run_block_time", F, 2, 0, {RUN_BLOCK,I}, {"block","row"}},
    {"runBlock_value", "psrt_run_block_value", F, 3, 0, {RUN_BLOCK,I,I}, {"block","row","channel"}},
    {"runBlock_times", "psrt_run_block_times", PS_LANG_FLOAT_ARRAY, 1, 0, {RUN_BLOCK}, {"block"}},
    {"runBlock_column", "psrt_run_block_column", PS_LANG_FLOAT_ARRAY, 2, 0, {RUN_BLOCK,I}, {"block","channel"}},
    {"runSnapshot_time", "psrt_run_snapshot_time", F, 1, 0, {RUN_SNAPSHOT}, {"snapshot"}},
    {"runSnapshot_paused", "psrt_run_snapshot_paused", PS_TYPE_BOOL, 1, 0, {RUN_SNAPSHOT}, {"snapshot"}},
    {"runSnapshot_channels", "psrt_run_snapshot_channels", I, 1, 0, {RUN_SNAPSHOT}, {"snapshot"}},
    {"runSnapshot_objects", "psrt_run_snapshot_objects", I, 1, 0, {RUN_SNAPSHOT}, {"snapshot"}},
    {"runSnapshot_points", "psrt_run_snapshot_points", I, 1, 0, {RUN_SNAPSHOT}, {"snapshot"}},
    {"runSnapshot_value", "psrt_run_snapshot_value", F, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","channel"}},
    {"runSnapshot_id", "psrt_run_snapshot_id", I, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_parent", "psrt_run_snapshot_parent", I, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_shape", "psrt_run_snapshot_shape", I, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_color", "psrt_run_snapshot_color", I, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_position", "psrt_run_snapshot_position", V3, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_size", "psrt_run_snapshot_size", V3, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_radius", "psrt_run_snapshot_radius", F, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_rotation", "psrt_run_snapshot_rotation", Q, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_point_first", "psrt_run_snapshot_point_first", I, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_point_count", "psrt_run_snapshot_point_count", I, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_text", "psrt_run_snapshot_text", S, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_point", "psrt_run_snapshot_point", V3, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"runSnapshot_world_point", "psrt_run_snapshot_world_point", V3, 3, 0, {RUN_SNAPSHOT,I,V3}, {"snapshot","index","local"}},
    {"runSnapshot_transform", "psrt_run_snapshot_transform", M4, 2, 0, {RUN_SNAPSHOT,I}, {"snapshot","index"}},
    {"StepInterval", "psrt_step_interval", STEP_INTERVAL, 2, 0, {F,F}, {"elapsed","nextStep"}},
    {"Rng", "psrt_rng_make", RNG, 1, 0, {I}, {"seed"}},
    {"rngForRun", "psrt_rng_for_run", RNG, 1, 1, {I}, {"stream"}},
    {"rngSample", "psrt_rng_sample", F, 2, 0, {RNG, DIST}, {"rng", "distribution"}},
    {"rngReseed", "psrt_rng_reseed", VOID, 2, 0, {RNG, I}, {"rng", "seed"}},
    {"rngReseedForRun", "psrt_rng_reseed_for_run", VOID, 2, 1,
     {RNG, I}, {"rng", "stream"}},
    {"Mat3", "psrt_mat3", M3, 3, 0, {V3, V3, V3}, {"column0", "column1", "column2"}},
    {"Mat4", "psrt_mat4", M4, 4, 0, {V4, V4, V4, V4}, {"column0", "column1", "column2", "column3"}},
    {"Bezier3", "psrt_bezier3", B3, 4, 0, {V3, V3, V3, V3},
     {"start", "control1", "control2", "end"}},
    {"Medium", "psrt_medium_make", MEDIUM, 2, 0, {F, F}, {"density", "viscosity"}},
    {"mediumAir", "psrt_medium_air", MEDIUM, 0, 0, {0}, {NULL}},
    {"mediumWater", "psrt_medium_water", MEDIUM, 0, 0, {0}, {NULL}},
    {"mediumVacuum", "psrt_medium_vacuum", MEDIUM, 0, 0, {0}, {NULL}},
    {"mediumDragForce", "psrt_medium_drag_force", V3, 4, 0,
     {MEDIUM, V3, F, F}, {"medium", "velocity", "coefficient", "area"}},
    {"mediumStokesDrag", "psrt_medium_stokes_drag", V3, 3, 0,
     {MEDIUM, V3, F}, {"medium", "velocity", "radius"}},
    {"Material", "psrt_material_make", MATERIAL, 3, 0, {F, F, F},
     {"density", "restitution", "friction"}},
    {"materialContactSolver", "psrt_material_contact_solver", SOLVER, 5, 0,
     {MATERIAL, I, F, F, F},
     {"material", "iterations", "bounceThreshold", "penetrationSlop", "correctionFraction"}},
    {"bezierPosition", "psrt_bezier_position", V3, 2, 0, {B3, F}, {"curve", "t"}},
    {"bezierTangent", "psrt_bezier_tangent", V3, 2, 0, {B3, F}, {"curve", "t"}},
    {"bezierSplitLeft", "psrt_bezier_split_left", B3, 2, 0, {B3, F}, {"curve", "t"}},
    {"bezierSplitRight", "psrt_bezier_split_right", B3, 2, 0, {B3, F}, {"curve", "t"}},
    {"identityMat3", "psrt_mat3_identity", M3, 0, 0, {0}, {NULL}},
    {"identityMat4", "psrt_mat4_identity", M4, 0, 0, {0}, {NULL}},
    {"elementMat3", "psrt_mat3_element", F, 3, 0, {M3, I, I}, {"matrix", "row", "column"}},
    {"elementMat4", "psrt_mat4_element", F, 3, 0, {M4, I, I}, {"matrix", "row", "column"}},
    {"multiplyMat3", "psrt_mat3_multiply", M3, 2, 0, {M3, M3}, {"left", "right"}},
    {"multiplyMat4", "psrt_mat4_multiply", M4, 2, 0, {M4, M4}, {"left", "right"}},
    {"transposeMat3", "psrt_mat3_transpose", M3, 1, 0, {M3}, {"matrix"}},
    {"transposeMat4", "psrt_mat4_transpose", M4, 1, 0, {M4}, {"matrix"}},
    {"applyMat3", "psrt_mat3_apply", V3, 2, 0, {M3, V3}, {"matrix", "vector"}},
    {"applyMat4", "psrt_mat4_apply", V4, 2, 0, {M4, V4}, {"matrix", "vector"}},
    {"inverseMat3", "psrt_mat3_inverse", M3, 2, 0, {M3, F}, {"matrix", "pivotTolerance"}},
    {"inverseMat4", "psrt_mat4_inverse", M4, 2, 0, {M4, F}, {"matrix", "pivotTolerance"}},
    {"translationMat4", "psrt_mat4_translation", M4, 1, 0, {V3}, {"translation"}},
    {"scaleMat4", "psrt_mat4_scale", M4, 1, 0, {V3}, {"scale"}},
    {"rotationMat4", "psrt_mat4_rotation", M4, 1, 0, {Q}, {"rotation"}},
    {"trsMat4", "psrt_mat4_trs", M4, 3, 0, {V3, Q, V3}, {"translation", "rotation", "scale"}},
    {"transformPoint", "psrt_matrix_point", V3, 2, 0, {M4, V3}, {"transform", "point"}},
    {"transformDirection", "psrt_matrix_direction", V3, 2, 0, {M4, V3}, {"transform", "direction"}},
    {"transformNormal", "psrt_matrix_normal", V3, 2, 0, {M4, V3}, {"transform", "normal"}},
    {"sweepSpheres", "psrt_sweep_spheres", SWEEP, 6, 0,
     {BODY, F, V3, BODY, F, V3}, {"bodyA", "radiusA", "displacementA", "bodyB", "radiusB", "displacementB"}},
    {"sweepSpherePlane", "psrt_sweep_plane", SWEEP, 5, 0,
     {BODY, F, V3, V3, V3}, {"body", "radius", "displacement", "point", "normal"}},
    {"sweepFraction", "psrt_sweep_fraction", F, 1, 0, {SWEEP}, {"sweep"}},
    {"sweepContacts", "psrt_sweep_contacts", CONTACTS, 1, 0, {SWEEP}, {"sweep"}},
    {"sphereBounds", "psrt_aabb_sphere", AABB, 2, 0, {BODY, F}, {"body", "radius"}},
    {"boxBounds", "psrt_aabb_box", AABB, 2, 0, {BODY, V3}, {"body", "size"}},
    {"sweptSphereBounds", "psrt_aabb_swept_sphere", AABB, 3, 0, {BODY, F, V3}, {"body", "radius", "displacement"}},
    {"collisionPairs", "psrt_collision_pairs", PS_LANG_PAIR_ARRAY, 1, 0, {PS_LANG_AABB_ARRAY}, {"bounds"}},
    {"contactConstraint", "psrt_graph_contact_make", CONTACT_CONSTRAINT, 4, 0,
     {CONTACTS, I, I, I}, {"contacts", "index", "bodyA", "bodyB"}},
    {"jointConstraint", "psrt_graph_joint_make", JOINT_CONSTRAINT, 3, 0,
     {JOINT, I, I}, {"joint", "bodyA", "bodyB"}},
    {"solveWarmContacts", "psrt_constraints_solve_warm", GRAPH_RESULT, 4, 0,
     {SOLVER,PS_LANG_BODY_ARRAY,PS_LANG_CONTACT_CONSTRAINT_ARRAY,PS_LANG_VEC3_ARRAY},
     {"solver","bodies","contacts","initialImpulses"}},
    {"solveConstraints", "psrt_constraints_solve", GRAPH_RESULT, 5, 0,
     {SOLVER, PS_LANG_BODY_ARRAY, PS_LANG_CONTACT_CONSTRAINT_ARRAY, PS_LANG_JOINT_CONSTRAINT_ARRAY, F},
     {"solver", "bodies", "contacts", "joints", "dt"}},
    {"constraintBody", "psrt_constraints_body", BODY, 2, 0, {GRAPH_RESULT, I}, {"result", "index"}},
    {"constraintBodies", "psrt_constraints_bodies", PS_LANG_BODY_ARRAY, 1, 0, {GRAPH_RESULT}, {"result"}},
    {"constraintContactImpulse", "psrt_constraints_contact_impulse", V3, 2, 0, {GRAPH_RESULT, I}, {"result", "index"}},
    {"constraintJointImpulse", "psrt_constraints_joint_impulse", V3, 2, 0, {GRAPH_RESULT, I}, {"result", "index"}},
    {"DistanceJoint", "psrt_distance_joint", JOINT, 4, 0,
     {V3, V3, F, F}, {"anchorA", "anchorB", "length", "stabilization"}},
    {"resolveJoint", "psrt_joint_resolve", JOINT_RESULT, 4, 0,
     {JOINT, BODY, BODY, F}, {"joint", "bodyA", "bodyB", "dt"}},
    {"ContactSolver", "psrt_contact_solver_make", SOLVER, 6, 0,
     {I, F, F, F, F, F}, {"iterations", "restitution", "friction", "bounceThreshold", "penetrationSlop", "correctionFraction"}},
    {"defaultContactSolver", "psrt_contact_solver_default", SOLVER, 0, 0, {0}, {NULL}},
    {"sphereContacts", "psrt_contacts_spheres", CONTACTS, 4, 0,
     {BODY, F, BODY, F}, {"bodyA", "radiusA", "bodyB", "radiusB"}},
    {"spherePlaneContacts", "psrt_contacts_sphere_plane", CONTACTS, 4, 0,
     {BODY, F, V3, V3}, {"body", "radius", "point", "normal"}},
    {"sphereBoxContacts", "psrt_contacts_sphere_box", CONTACTS, 4, 0,
     {BODY, F, BODY, V3}, {"sphere", "radius", "box", "size"}},
    {"boxPlaneContacts", "psrt_contacts_box_plane", CONTACTS, 4, 0,
     {BODY, V3, V3, V3}, {"body", "size", "point", "normal"}},
    {"boxContacts", "psrt_contacts_boxes", CONTACTS, 4, 0,
     {BODY, V3, BODY, V3}, {"bodyA", "sizeA", "bodyB", "sizeB"}},
    {"contactPoint", "psrt_contact_point", V3, 2, 0, {CONTACTS, I}, {"contacts", "index"}},
    {"contactNormal", "psrt_contact_normal", V3, 2, 0, {CONTACTS, I}, {"contacts", "index"}},
    {"contactPenetration", "psrt_contact_penetration", F, 2, 0, {CONTACTS, I}, {"contacts", "index"}},
    {"resolveContacts", "psrt_contacts_resolve", RESULT, 4, 0,
     {CONTACTS, BODY, BODY, SOLVER}, {"contacts", "bodyA", "bodyB", "solver"}},
    {"resolveSingleContact", "psrt_contact_resolve_single", RESULT, 5, 0,
     {CONTACTS, BODY, BODY, F, F}, {"contacts", "bodyA", "bodyB", "restitution", "friction"}},
    {"contactImpulse", "psrt_contact_impulse", V3, 2, 0, {RESULT, I}, {"result", "index"}},
    {"sphereBody", "psrt_body_sphere", BODY, 2, 0, {F, F}, {"mass", "radius"}},
    {"boxBody", "psrt_body_box", BODY, 2, 0, {F, V3}, {"mass", "size"}},
    {"bodySetState", "psrt_body_set_state", VOID, 5, 0,
     {BODY, V3, V3, Q, V3}, {"body", "position", "velocity", "orientation", "angularVelocity"}},
    {"bodyApplyImpulse", "psrt_body_apply_impulse", VOID, 3, 0,
     {BODY, V3, V3}, {"body", "impulse", "point"}},
    {"bodyStep", "psrt_body_step", VOID, 4, 0,
     {BODY, V3, V3, F}, {"body", "force", "torque", "dt"}},
    {"bodyKineticEnergy", "psrt_body_kinetic_energy", F, 1, 0, {BODY}, {"body"}},
    {"bodyPointVelocity", "psrt_body_point_velocity", V3, 2, 0,
     {BODY, V3}, {"body", "point"}},
    {"bodyForceTorque", "psrt_body_force_torque", V3, 3, 0,
     {BODY, V3, V3}, {"body", "force", "point"}},
    {"selectSeries", "psra_select", PS_LANG_SERIES_ARRAY, 3, 2,
     {PS_LANG_SERIES_ARRAY, R, F}, {"columns", "selector", "accepted"}},
    {"maskSeries","psra_mask",R,3,2,{R,R,F},{"input","selector","accepted"}},
    {"seriesValidity","psra_validity",R,1,2,{R},{"input"}},
    {"seriesHasMask","psra_has_mask",PS_TYPE_BOOL,1,2,{R},{"input"}},
    {"seriesIsValid","psra_is_valid",PS_TYPE_BOOL,2,2,{R,I},{"input","index"}},
    {"seriesFromValues", "psra_series_from_values", R, 3, 2,
     {PS_LANG_FLOAT_ARRAY, U, S}, {"values", "unit", "name"}},
    {"seriesAlignedValues", "psra_series_aligned_values", R, 4, 2,
     {R, PS_LANG_FLOAT_ARRAY, U, S}, {"anchor", "values", "unit", "name"}},
    {"Table", "psra_table", TABLE, 3, 2,
     {S, PS_LANG_STRING_ARRAY, PS_LANG_UNIT_ARRAY}, {"title", "labels", "units"}},
    {"tableRow", "psra_row", VOID, 3, 2,
     {TABLE, S, PS_LANG_QUANTITY_ARRAY}, {"table", "label", "values"}},
    {"exportTable", "psra_table_csv", VOID, 2, 2, {TABLE, S}, {"table", "suffix"}},
    {"constantDistribution", "psrt_distribution_constant", DIST, 1, 0, {F}, {"value"}},
    {"uniformDistribution", "psrt_distribution_uniform", DIST, 2, 0, {F, F}, {"min", "max"}},
    {"normalDistribution", "psrt_distribution_normal", DIST, 2, 0,
     {F, F}, {"mean", "standardDeviation"}},
    {"distributionMean", "psrt_distribution_mean", F, 1, 0, {DIST}, {"distribution"}},
    {"distributionDeviation", "psrt_distribution_deviation", F, 1, 0, {DIST}, {"distribution"}},
    {"SensorConfig", "psrt_sensor_config", CONFIG, 10, 0,
     {U, F, F, F, F, F, DIST, F, F, F},
     {"unit", "rateHz", "startTime", "resolution", "offset", "driftPerSecond", "noise",
      "dropoutProbability", "uncertaintyAbsolute", "uncertaintyRelative"}},
    {"Sensor", "psrt_sensor_init", SENSOR, 2, 0, {CONFIG, I}, {"config", "seed"}},
    {"sensorForRun", "psrt_sensor_for_run", SENSOR, 2, 1, {CONFIG, I}, {"config", "stream"}},
    {"sensorReset", "psrt_sensor_reset", VOID, 2, 0, {SENSOR, I}, {"sensor", "seed"}},
    {"sensorResetForRun", "psrt_sensor_reset_for_run", VOID, 2, 1,
     {SENSOR, I}, {"sensor", "stream"}},
    {"sensorRead", "psrt_sensor_read", SAMPLE, 3, 0,
     {SENSOR, F, QUANTITY}, {"sensor", "time", "truth"}},
    {"sensorNextTime", "psrt_sensor_next_time", F, 1, 0, {SENSOR}, {"sensor"}},
    {"measurementValid", "psrt_measurement_valid", PS_TYPE_BOOL, 1, 0, {SAMPLE}, {"measurement"}},
    {"measurementDue", "psrt_measurement_due", PS_TYPE_BOOL, 1, 0, {SAMPLE}, {"measurement"}},
    {"measurementDropped", "psrt_measurement_dropped", PS_TYPE_BOOL, 1, 0, {SAMPLE}, {"measurement"}},
    {"sin", "psrt_sin", F, 1, 0, {F}, {"angle"}},
    {"cos", "psrt_cos", F, 1, 0, {F}, {"angle"}},
    {"tan", "psrt_tan", F, 1, 0, {F}, {"angle"}},
    {"asin", "psrt_asin", F, 1, 0, {F}, {"value"}},
    {"acos", "psrt_acos", F, 1, 0, {F}, {"value"}},
    {"atan", "psrt_atan", F, 1, 0, {F}, {"value"}},
    {"atan2", "psrt_atan2", F, 2, 0, {F, F}, {"y", "x"}},
    {"exp", "psrt_exp", F, 1, 0, {F}, {"value"}},
    {"log", "psrt_log", F, 1, 0, {F}, {"value"}},
    {"log10", "psrt_log10", F, 1, 0, {F}, {"value"}},
    {"pow", "psrt_pow", F, 2, 0, {F, F}, {"base", "exponent"}},
    {"hypot", "psrt_hypot", F, 2, 0, {F, F}, {"x", "y"}},
    {"sqrt", "psrt_sqrt", F, 1, 0, {F}, {"value"}},
    {"abs", "psrt_abs", F, 1, 0, {F}, {"value"}},
    {"floor", "psrt_floor", F, 1, 0, {F}, {"value"}},
    {"ceil", "psrt_ceil", F, 1, 0, {F}, {"value"}},
    {"round", "psrt_round", F, 1, 0, {F}, {"value"}},
    {"min", "psrt_min", F, 2, 0, {F, F}, {"left", "right"}},
    {"max", "psrt_max", F, 2, 0, {F, F}, {"left", "right"}},
    {"clamp", "psrt_clamp", F, 3, 0, {F, F, F}, {"value", "lower", "upper"}},
    {"intAbs", "psrt_int_abs", I, 1, 0, {I}, {"value"}},
    {"intMin", "psrt_int_min", I, 2, 0, {I, I}, {"left", "right"}},
    {"intMax", "psrt_int_max", I, 2, 0, {I, I}, {"left", "right"}},
    {"intClamp", "psrt_int_clamp", I, 3, 0, {I, I, I}, {"value", "lower", "upper"}},
    {"linearSolve", "psrt_linear_solve", PS_LANG_FLOAT_ARRAY, 3, 0,
     {PS_LANG_FLOAT_ARRAY, PS_LANG_FLOAT_ARRAY, F},
     {"coefficients", "rhs", "pivotTolerance"}},
    {"rootBisect", "psrt_root_bisect", F, 6, 0,
     {PS_TYPE_FUNCTION, F, F, F, F, I},
     {"function", "lower", "upper", "absoluteTolerance", "relativeTolerance", "maxIterations"}},
    {"minimizeGolden", "psrt_minimize_golden", F, 6, 0,
     {PS_TYPE_FUNCTION, F, F, F, F, I},
     {"function", "lower", "upper", "absoluteTolerance", "relativeTolerance", "maxIterations"}},
    {"rootBisectReported", "psrt_root_bisect_reported", SCALAR_RESULT, 6, 0,
     {PS_TYPE_FUNCTION, F, F, F, F, I},
     {"function", "lower", "upper", "absoluteTolerance", "relativeTolerance", "maxIterations"}},
    {"minimizeGoldenReported", "psrt_minimize_golden_reported", SCALAR_RESULT, 6, 0,
     {PS_TYPE_FUNCTION, F, F, F, F, I},
     {"function", "lower", "upper", "absoluteTolerance", "relativeTolerance", "maxIterations"}},
    {"eulerStep", "psrt_ode_euler", PS_LANG_FLOAT_ARRAY, 4, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F},
     {"derivative", "state", "time", "dt"}},
    {"rk4Step", "psrt_ode_rk4", PS_LANG_FLOAT_ARRAY, 4, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F},
     {"derivative", "state", "time", "dt"}},
    {"rk45Integrate", "psrt_ode_rk45", PS_LANG_FLOAT_ARRAY, 7, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F, F, F, I},
     {"derivative", "state", "start", "end", "absoluteTolerance",
      "relativeTolerance", "maxSteps"}},
    {"rk45IntegrateWithSteps", "psrt_ode_rk45_with_steps", PS_LANG_FLOAT_ARRAY, 10, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F, F, F, I, F, F, F},
     {"derivative", "state", "start", "end", "absoluteTolerance",
      "relativeTolerance", "maxSteps", "initialStep", "minimumStep", "maximumStep"}},
    {"rk45IntegrateWithTolerances", "psrt_ode_rk45_with_tolerances", PS_LANG_FLOAT_ARRAY, 10, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F, PS_LANG_FLOAT_ARRAY, F, I, F, F, F},
     {"derivative", "state", "start", "end", "absoluteTolerances",
      "relativeTolerance", "maxSteps", "initialStep", "minimumStep", "maximumStep"}},
    {"rk45StepReported", "psrt_ode_rk45_step_reported", ODE_RESULT, 10, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F, F, F, I, F, F, F},
     {"derivative", "state", "start", "end", "absoluteTolerance",
      "relativeTolerance", "maxSteps", "initialStep", "minimumStep", "maximumStep"}},
    {"rk45IntegrateReported", "psrt_ode_rk45_reported", ODE_RESULT, 10, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F, F, F, I, F, F, F},
     {"derivative", "state", "start", "end", "absoluteTolerance",
      "relativeTolerance", "maxSteps", "initialStep", "minimumStep", "maximumStep"}},
    {"rk45IntegrateWithTolerancesReported", "psrt_ode_rk45_with_tolerances_reported",
     ODE_RESULT, 10, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F, PS_LANG_FLOAT_ARRAY, F, I, F, F, F},
     {"derivative", "state", "start", "end", "absoluteTolerances",
      "relativeTolerance", "maxSteps", "initialStep", "minimumStep", "maximumStep"}},
    {"verletStep", "psrt_verlet_step", PS_LANG_FLOAT_ARRAY, 4, 0,
     {PS_LANG_ODE_CALLBACK, PS_LANG_FLOAT_ARRAY, F, F},
     {"acceleration", "phase", "time", "dt"}},
    {"Vec2", "psrt_vec2", V2, 2, 0, {F, F}, {"x", "y"}},
    {"Vec3", "psrt_vec3", V3, 3, 0, {F, F, F}, {"x", "y", "z"}},
    {"Vec4", "psrt_vec4", V4, 4, 0, {F, F, F, F}, {"x", "y", "z", "w"}},
    {"Quat", "psrt_quat", Q, 4, 0, {F, F, F, F}, {"x", "y", "z", "w"}},
    {"axisAngle", "psrt_axis_angle", Q, 2, 0, {V3, F}, {"axis", "angle"}},
    {"rotate", "psrt_rotate", V3, 2, 0, {Q, V3}, {"rotation", "vector"}},
    {"normalizeQuat", "psrt_rotation", Q, 1, 0, {Q}, {"rotation"}},
    {"conjugateQuat", "psrt_quat_conjugate", Q, 1, 0, {Q}, {"value"}},
    {"multiplyQuat", "psrt_quat_multiply", Q, 2, 0, {Q, Q}, {"left", "right"}},
    {"slerpQuat", "psrt_quat_slerp", Q, 3, 0, {Q, Q, F}, {"start", "end", "fraction"}},
    {"dot2", "psrt_dot2", F, 2, 0, {V2, V2}, {"left", "right"}},
    {"dot3", "psrt_dot3", F, 2, 0, {V3, V3}, {"left", "right"}},
    {"dot4", "psrt_dot4", F, 2, 0, {V4, V4}, {"left", "right"}},
    {"cross2", "psrt_cross2", F, 2, 0, {V2, V2}, {"left", "right"}},
    {"cross3", "psrt_cross3", V3, 2, 0, {V3, V3}, {"left", "right"}},
    {"length2", "psrt_length2", F, 1, 0, {V2}, {"vector"}},
    {"length3", "psrt_length3", F, 1, 0, {V3}, {"vector"}},
    {"length4", "psrt_length4", F, 1, 0, {V4}, {"vector"}},
    {"normalize2", "psrt_normalize2", V2, 1, 0, {V2}, {"vector"}},
    {"normalize3", "psrt_normalize3", V3, 1, 0, {V3}, {"vector"}},
    {"normalize4", "psrt_normalize4", V4, 1, 0, {V4}, {"vector"}},
    {"springForce",
     "psrt_spring_force",
     V3,
     7,
     0,
     {V3, V3, V3, V3, F, F, F},
     {"position", "velocity", "anchor", "anchorVelocity", "stiffness", "restLength", "damping"}},
    {"stokesDrag",
     "psrt_stokes_drag",
     V3,
     3,
     0,
     {V3, F, F},
     {"relativeVelocity", "viscosity", "radius"}},
    {"quadraticDrag",
     "psrt_quadratic_drag",
     V3,
     4,
     0,
     {V3, F, F, F},
     {"relativeVelocity", "density", "radius", "coefficient"}},
    {"buoyancyForce",
     "psrt_buoyancy_force",
     V3,
     3,
     0,
     {F, F, V3},
     {"density", "volume", "gravity"}},
    {"sphereSubmersion", "psrt_sphere_submersion", SUBMERSION, 2, 0,
     {F, F}, {"radius", "centerHeight"}},
    {"symplectic", "psrt_symplectic", V2, 3, 0, {V2, F, F}, {"state", "acceleration", "dt"}},
    {"Unit",
     "psrt_unit",
     U,
     9,
     0,
     {I, I, I, I, I, I, I, F, S},
     {"length", "mass", "time", "current", "temperature", "amount", "luminosity", "scale",
      "symbol"}},
    {"convert", "psrt_convert", F, 3, 0, {F, U, U}, {"value", "from", "to"}},
    {"Quantity", "psrt_quantity", QUANTITY, 2, 0, {F, U}, {"value", "unit"}},
    {"convertQuantity", "psrt_quantity_convert", QUANTITY, 2, 0, {QUANTITY, U}, {"value", "to"}},
    {"addQuantity", "psrt_quantity_add", QUANTITY, 2, 0, {QUANTITY, QUANTITY}, {"left", "right"}},
    {"subtractQuantity", "psrt_quantity_subtract", QUANTITY, 2, 0, {QUANTITY, QUANTITY}, {"left", "right"}},
    {"multiplyQuantity", "psrt_quantity_multiply", QUANTITY, 3, 0, {QUANTITY, QUANTITY, S}, {"left", "right", "symbol"}},
    {"divideQuantity", "psrt_quantity_divide", QUANTITY, 3, 0, {QUANTITY, QUANTITY, S}, {"left", "right", "symbol"}},
    {"multiplyUnit", "psrt_unit_multiply", U, 3, 0, {U, U, S}, {"left", "right", "symbol"}},
    {"divideUnit", "psrt_unit_divide", U, 3, 0, {U, U, S}, {"left", "right", "symbol"}},
    {"powerUnit", "psrt_unit_power", U, 3, 0, {U, I, S}, {"unit", "exponent", "symbol"}},
    {"compatibleUnit", "psrt_unit_compatible", PS_TYPE_BOOL, 2, 0, {U, U}, {"left", "right"}},
    {"Channel", "psrt_add_channel", C, 3, 1, {S, U, S}, {"name", "unit", "description"}},
    {"sample", "psrt_sample", VOID, 2, 1, {C, F}, {"channel", "value"}},
    {"sphere", "psrt_sphere", VOID, 4, 1, {V3, F, I, I}, {"center", "radius", "color", "id"}},
    {"line", "psrt_line", VOID, 5, 1, {V3, V3, F, I, I}, {"start", "end", "radius", "color", "id"}},
    {"arrow",
     "psrt_arrow",
     VOID,
     5,
     1,
     {V3, V3, F, I, I},
     {"start", "end", "radius", "color", "id"}},
    {"point", "psrt_point", VOID, 4, 1, {V3, F, I, I}, {"position", "radius", "color", "id"}},
    {"polyline", "psrt_polyline", VOID, 4, 1,
     {PS_LANG_VEC3_ARRAY, F, I, I}, {"points", "radius", "color", "id"}},
    {"box",
     "psrt_box",
     VOID,
     5,
     1,
     {V3, V3, Q, I, I},
     {"center", "size", "rotation", "color", "id"}},
    {"plane",
     "psrt_plane",
     VOID,
     5,
     1,
     {V3, V2, Q, I, I},
     {"center", "size", "rotation", "color", "id"}},
    {"label", "psrt_label", VOID, 4, 1, {V3, S, I, I}, {"position", "text", "color", "id"}},
    {"group", "psrt_group", VOID, 3, 1, {S,I,I}, {"name","id","parent"}},
    {"sceneFrame","psrt_scene_frame",VOID,6,1,{S,I,I,V3,Q,V3},{"name","id","parent","translation","rotation","scale"}},
    {"sceneTransform","psrt_scene_transform",M4,1,1,{I},{"index"}},
    {"sceneWorldPoint","psrt_scene_world_point",V3,2,1,{I,V3},{"index","point"}},
    {"sceneParent", "psrt_scene_parent", VOID, 2, 1, {I,I}, {"child","parent"}},
    {"Diagnostic","psrt_diagnostic_make",DIAGNOSTIC,7,0,{I,S,S,S,I,I,S},{"code","operation","argument","source","line","column","message"}},
    {"diagnosticHere","psrt_diagnostic_here",DIAGNOSTIC,4,0,{I,S,S,S},{"code","operation","argument","message"}},
    {"emptyDiagnostic","psrt_diagnostic_empty",DIAGNOSTIC,0,0,{0},{0}},
    {"diagnosticValid","psrt_diagnostic_valid",PS_TYPE_BOOL,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticCode","psrt_diagnostic_code",I,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticLine","psrt_diagnostic_line",I,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticColumn","psrt_diagnostic_column",I,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticOperation","psrt_diagnostic_operation",S,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticArgument","psrt_diagnostic_argument",S,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticSource","psrt_diagnostic_source",S,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticMessage","psrt_diagnostic_message",S,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticFormatted","psrt_diagnostic_formatted",S,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticEncoded","psrt_diagnostic_encoded",PS_LANG_INT_ARRAY,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"diagnosticDecoded","psrt_diagnostic_decoded",DIAGNOSTIC,1,0,{PS_LANG_INT_ARRAY},{"bytes"}},
    {"saveDiagnostic","psrt_diagnostic_save",VOID,2,0,{DIAGNOSTIC,S},{"diagnostic","path"}},
    {"loadDiagnostic","psrt_diagnostic_load",DIAGNOSTIC,1,0,{S},{"path"}},
    {"raiseDiagnostic","psrt_diagnostic_raise",VOID,1,0,{DIAGNOSTIC},{"diagnostic"}},
    {"currentDiagnostic","psrt_current_diagnostic",DIAGNOSTIC,0,1,{0},{0}},
    {"logDebug","psrt_log_debug",PS_TYPE_BOOL,1,1,{S},{"message"}},
    {"logInfo","psrt_log_info",PS_TYPE_BOOL,1,1,{S},{"message"}},
    {"logWarning","psrt_log_warning",PS_TYPE_BOOL,1,1,{S},{"message"}},
    {"logError","psrt_log_error",PS_TYPE_BOOL,1,1,{S},{"message"}},
    {"metadata", "psrt_metadata", VOID, 1, 1, {S}, {"text"}},
    {"simulationTime", "psrt_time", F, 0, 1, {0}, {0}},
    {"runSeed", "psrt_run_seed", I, 0, 1, {0}, {0}},
    {"parameter", "psrt_parameter", F, 5, 1,
     {S, F, F, F, S}, {"name", "default", "minimum", "maximum", "description"}},
    {"parameterWithUnit", "psrt_parameter_unit", F, 6, 1,
     {S,U,F,F,F,S}, {"name","unit","default","minimum","maximum","description"}},
    {"randomUniform", "psrt_random_uniform", F, 2, 1, {F, F}, {"min", "max"}},
    {"randomNormal", "psrt_random_normal", F, 2, 1, {F, F}, {"mean", "standardDeviation"}},
    {"inputPath", "psra_input_path", S, 1, 2, {I}, {"index"}},
    {"inputCount", "psra_input_count", I, 0, 2, {0}, {0}},
    {"Dataset", "psra_dataset", D, 1, 2, {I}, {"index"}},
    {"datasetSampleCount", "psra_dataset_sample_count", I, 1, 2, {D}, {"dataset"}},
    {"datasetChannelCount", "psra_dataset_channel_count", I, 1, 2, {D}, {"dataset"}},
    {"datasetChannelName", "psra_channel_name", S, 2, 2, {D, I}, {"dataset", "index"}},
    {"datasetChannelUnitSymbol", "psra_channel_unit_symbol", S, 2, 2,
     {D, I}, {"dataset", "index"}},
    {"datasetChannelDescription", "psra_channel_description", S, 2, 2,
     {D, I}, {"dataset", "index"}},
    {"datasetChannelExponent", "psra_channel_exponent", I, 3, 2,
     {D, I, I}, {"dataset", "index", "axis"}},
    {"datasetRecovered", "psra_dataset_recovered", PS_TYPE_BOOL, 1, 2,
     {D}, {"dataset"}},
    {"datasetMetadata", "psra_dataset_metadata", S, 1, 2, {D}, {"dataset"}},
    {"series", "psra_series", R, 2, 2, {D, S}, {"dataset", "name"}},
    {"seriesCount", "psra_count", I, 1, 2, {R}, {"input"}},
    {"seriesName", "psra_series_name", S, 1, 2, {R}, {"input"}},
    {"seriesUnitSymbol", "psra_series_unit_symbol", S, 1, 2, {R}, {"input"}},
    {"seriesUnitScale", "psra_series_unit_scale", F, 1, 2, {R}, {"input"}},
    {"seriesExponent", "psra_series_exponent", I, 2, 2,
     {R, I}, {"input", "axis"}},
    {"seriesAligned", "psra_series_aligned", PS_TYPE_BOOL, 2, 2,
     {R, R}, {"left", "right"}},
    {"sliceSeries", "psra_slice", R, 3, 2, {R, I, I}, {"input", "first", "count"}},
    {"seriesValue", "psra_value", F, 2, 2, {R, I}, {"input", "index"}},
    {"seriesValues", "psra_values", PS_LANG_FLOAT_ARRAY, 3, 2,
     {R, I, I}, {"input", "first", "count"}},
    {"mean", "psra_mean", F, 1, 2, {R}, {"input"}},
    {"stddev", "psra_stddev", F, 1, 2, {R}, {"input"}},
    {"quantile", "psra_quantile", F, 2, 2, {R, F}, {"input", "probability"}},
    {"minimum", "psra_minimum", F, 1, 2, {R}, {"input"}},
    {"maximum", "psra_maximum", F, 1, 2, {R}, {"input"}},
    {"derivative", "psra_derivative", R, 2, 2, {R, R}, {"y", "x"}},
    {"integral", "psra_integral", R, 4, 2, {R, R, F, U}, {"y", "x", "initial", "unit"}},
    {"affine", "psra_affine", R, 4, 2, {R, F, F, U}, {"input", "factor", "offset", "unit"}},
    {"addSeries", "psra_add", R, 2, 2, {R, R}, {"left", "right"}},
    {"subtractSeries", "psra_subtract", R, 2, 2, {R, R}, {"left", "right"}},
    {"multiplySeries", "psra_multiply", R, 2, 2, {R, R}, {"left", "right"}},
    {"divideSeries", "psra_divide", R, 2, 2, {R, R}, {"left", "right"}},
    {"resampleLinear", "psra_linear", R, 3, 2, {R, R, R}, {"y", "x", "targetX"}},
    {"resampleNearest", "psra_nearest", R, 3, 2, {R, R, R}, {"y", "x", "targetX"}},
    {"resamplePrevious", "psra_previous", R, 3, 2, {R, R, R}, {"y", "x", "targetX"}},
    {"resamplePchip", "psra_pchip", R, 3, 2, {R, R, R}, {"y", "x", "targetX"}},
    {"movingAverage", "psra_average", R, 2, 2, {R, I}, {"input", "window"}},
    {"release", "psra_release", VOID, 1, 2, {R}, {"input"}},
    {"closeDataset", "psra_close", VOID, 1, 2, {D}, {"dataset"}},
    {"report", "psra_report", VOID, 1, 2, {S}, {"title"}},
    {"plot", "psra_plot", P, 4, 2, {R, R, S, S}, {"x", "y", "title", "label"}},
    {"curve", "psra_curve", VOID, 4, 2, {P, R, R, S}, {"plot", "x", "y", "label"}},
    {"points", "psra_points", VOID, 4, 2, {P, R, R, S}, {"plot", "x", "y", "label"}},
    {"histogram", "psra_histogram", P, 3, 2, {R, S, I}, {"input", "title", "bins"}},
    {"exportSeries", "psra_export", VOID, 3, 2, {R, R, S}, {"x", "y", "suffix"}},
    {"exportColumns", "psra_export_columns", VOID, 2, 2,
     {PS_LANG_SERIES_ARRAY, S}, {"columns", "suffix"}},
    {"exportPlot", "psra_svg", VOID, 2, 2, {P, S}, {"plot", "suffix"}}};
static const ps_lang_builtin *library_find(const void *name, size_t length, size_t *binding) {
    for (size_t i = 0; i < sizeof(library) / sizeof(library[0]); i++)
        if (length == strlen(library[i].name) && memcmp(name, library[i].name, length) == 0) {
            *binding = PS_LANG_LIBRARY_BASE + i;
            return &library[i];
        }
    return NULL;
}
const ps_lang_builtin *ps_lang_builtin_get(size_t binding) {
    size_t index = binding - PS_LANG_LIBRARY_BASE;
    return binding >= PS_LANG_LIBRARY_BASE && index < sizeof(library) / sizeof(library[0])
               ? &library[index]
               : NULL;
}
static const ps_lang_method methods[] = {
    {"parameter","batchParameter",BATCH,0},
    {"sweep","batchSweep",BATCH,0},
    {"target","batchTarget",BATCH,0},
    {"adaptive","batchAdaptive",BATCH,0},
    {"limits","batchLimits",BATCH,0},
    {"source","batchSource",BATCH,0},
    {"run","batchRun",BATCH,0},
    {"runUntil","batchRunUntil",BATCH,0},
    {"requireSuccess","batchRequireSuccess",BATCH,0},
    {"series","batchSeries",BATCH,0},
    {"directory","batch_directory",BATCH,0},
    {"error","batch_error",BATCH,0},
    {"module","batch_module",BATCH,0},
    {"runs","batch_runs",BATCH,0},
    {"steps","batch_steps",BATCH,0},
    {"workers","batch_workers",BATCH,0},
    {"seed","batch_seed",BATCH,0},
    {"completed","batch_completed",BATCH,0},
    {"started","batch_started",BATCH,0},
    {"reused","batch_reused",BATCH,0},
    {"valid","batch_valid",BATCH,0},
    {"peakActive","batch_peakActive",BATCH,0},
    {"code","batch_code",BATCH,0},
    {"dt","batch_dt",BATCH,0},
    {"endTime","batch_endTime",BATCH,0},
    {"executed","batch_executed",BATCH,0},
    {"cancelled","batch_cancelled",BATCH,0},
    {"unit","batch_unit",BATCH,0},
    {"values","batch_values",BATCH,0},
    {"statuses","batch_statuses",BATCH,0},
    {"value","batch_value",BATCH,0},
    {"status","batch_status",BATCH,0},
    {"finished","batch_finished",BATCH,0},
    {"runPath","batch_runPath",BATCH,0},
    {"reset","worldReset",WORLD,0},
    {"solve","worldSolve",WORLD,0},
    {"solveWarm","solveWarmContacts",SOLVER,0},
    {"id","collider_id",COLLIDER,0},
    {"body","collider_body",COLLIDER,0},
    {"shape","collider_shape",COLLIDER,0},
    {"size","collider_size",COLLIDER,0},
    {"normal","collider_normal",COLLIDER,0},
    {"bodyCount","world_bodyCount",WORLD,0},
    {"colliderCount","world_colliderCount",WORLD,0},
    {"contactCount","world_contactCount",WORLD,0},
    {"matched","world_matched",WORLD,0},
    {"created","world_created",WORLD,0},
    {"ended","world_ended",WORLD,0},
    {"warmed","world_warmed",WORLD,0},
    {"maxNormalError","world_maxNormalError",WORLD,0},
    {"maxProjectionError","world_maxProjectionError",WORLD,0},
    {"dt","world_dt",WORLD,0},
    {"body","world_body",WORLD,0},
    {"contactIdA","world_contactIdA",WORLD,0},
    {"contactIdB","world_contactIdB",WORLD,0},
    {"contactPoint","world_contactPoint",WORLD,0},
    {"contactNormal","world_contactNormal",WORLD,0},
    {"localAnchorA","world_localAnchorA",WORLD,0},
    {"localAnchorB","world_localAnchorB",WORLD,0},
    {"contactImpulse","world_contactImpulse",WORLD,0},
    {"contactPenetration","world_contactPenetration",WORLD,0},
    {"bodies","worldBodies",WORLD,0},
    {"close","runIndexClose",RUN_INDEX,PS_LANG_METHOD_MUTATING},
    {"isOpen","runIndex_is_open",RUN_INDEX,0},
    {"sampleCount","runIndex_samples",RUN_INDEX,0},
    {"snapshotCount","runIndex_snapshots",RUN_INDEX,0},
    {"checkpointCount","runIndex_checkpoints",RUN_INDEX,0},
    {"channelCount","runIndex_channels",RUN_INDEX,0},
    {"isComplete","runIndex_complete",RUN_INDEX,0},
    {"isPersisted","runIndex_persisted",RUN_INDEX,0},
    {"metadata","runIndex_metadata",RUN_INDEX,0},
    {"channelName","runIndex_name",RUN_INDEX,0},
    {"channelSymbol","runIndex_symbol",RUN_INDEX,0},
    {"channelDescription","runIndex_description",RUN_INDEX,0},
    {"channelDimension","runIndexDimension",RUN_INDEX,0},
    {"read","runIndexRead",RUN_INDEX,0},
    {"snapshot","runIndexSnapshot",RUN_INDEX,0},
    {"count","runBlock_count",RUN_BLOCK,0},
    {"channelCount","runBlock_channels",RUN_BLOCK,0},
    {"time","runBlock_time",RUN_BLOCK,0},
    {"value","runBlock_value",RUN_BLOCK,0},
    {"times","runBlock_times",RUN_BLOCK,0},
    {"column","runBlock_column",RUN_BLOCK,0},
    {"time","runSnapshot_time",RUN_SNAPSHOT,0},
    {"isPaused","runSnapshot_paused",RUN_SNAPSHOT,0},
    {"channelCount","runSnapshot_channels",RUN_SNAPSHOT,0},
    {"objectCount","runSnapshot_objects",RUN_SNAPSHOT,0},
    {"pointCount","runSnapshot_points",RUN_SNAPSHOT,0},
    {"value","runSnapshot_value",RUN_SNAPSHOT,0},
    {"objectId","runSnapshot_id",RUN_SNAPSHOT,0},
    {"parentId","runSnapshot_parent",RUN_SNAPSHOT,0},
    {"shape","runSnapshot_shape",RUN_SNAPSHOT,0},
    {"color","runSnapshot_color",RUN_SNAPSHOT,0},
    {"position","runSnapshot_position",RUN_SNAPSHOT,0},
    {"size","runSnapshot_size",RUN_SNAPSHOT,0},
    {"radius","runSnapshot_radius",RUN_SNAPSHOT,0},
    {"orientation","runSnapshot_rotation",RUN_SNAPSHOT,0},
    {"pointFirst","runSnapshot_point_first",RUN_SNAPSHOT,0},
    {"objectPointCount","runSnapshot_point_count",RUN_SNAPSHOT,0},
    {"text","runSnapshot_text",RUN_SNAPSHOT,0},
    {"point","runSnapshot_point",RUN_SNAPSHOT,0},
    {"worldPoint","runSnapshot_world_point",RUN_SNAPSHOT,0},
    {"transform","runSnapshot_transform",RUN_SNAPSHOT,0},
    {"isValid","diagnosticValid",DIAGNOSTIC,0}, {"code","diagnosticCode",DIAGNOSTIC,0},
    {"line","diagnosticLine",DIAGNOSTIC,0}, {"column","diagnosticColumn",DIAGNOSTIC,0},
    {"operation","diagnosticOperation",DIAGNOSTIC,0}, {"argument","diagnosticArgument",DIAGNOSTIC,0},
    {"source","diagnosticSource",DIAGNOSTIC,0}, {"message","diagnosticMessage",DIAGNOSTIC,0},
    {"formatted","diagnosticFormatted",DIAGNOSTIC,0}, {"encoded","diagnosticEncoded",DIAGNOSTIC,0},
    {"save","saveDiagnostic",DIAGNOSTIC,0}, {"raise","raiseDiagnostic",DIAGNOSTIC,0},
{"row", "tableRow", TABLE, 0},
                                         {"sample", "rngSample", RNG, PS_LANG_METHOD_MUTATING},
                                         {"reseed", "rngReseed", RNG, PS_LANG_METHOD_MUTATING},
                                         {"reseedForRun", "rngReseedForRun", RNG, PS_LANG_METHOD_MUTATING},
                                         {"element", "elementMat3", M3, 0},
                                         {"element", "elementMat4", M4, 0},
                                         {"multiplied", "multiplyMat3", M3, 0},
                                         {"multiplied", "multiplyMat4", M4, 0},
                                         {"transposed", "transposeMat3", M3, 0},
                                         {"transposed", "transposeMat4", M4, 0},
                                         {"applied", "applyMat3", M3, 0},
                                         {"applied", "applyMat4", M4, 0},
                                         {"inverse", "inverseMat3", M3, 0},
                                         {"inverse", "inverseMat4", M4, 0},
                                         {"position", "bezierPosition", B3, 0},
                                         {"tangent", "bezierTangent", B3, 0},
                                         {"splitLeft", "bezierSplitLeft", B3, 0},
                                         {"splitRight", "bezierSplitRight", B3, 0},
                                         {"dragForce", "mediumDragForce", MEDIUM, 0},
                                         {"stokesDrag", "mediumStokesDrag", MEDIUM, 0},
                                         {"contactSolver", "materialContactSolver", MATERIAL, 0},
                                         {"transformPoint", "transformPoint", M4, 0},
                                         {"transformDirection", "transformDirection", M4, 0},
                                         {"transformNormal", "transformNormal", M4, 0},
                                         {"fraction", "sweepFraction", SWEEP, 0},
                                         {"contacts", "sweepContacts", SWEEP, 0},
                                         {"constraint", "contactConstraint", CONTACTS, 0},
                                         {"constraint", "jointConstraint", JOINT, 0},
                                         {"solve", "solveConstraints", SOLVER, 0},
                                         {"body", "constraintBody", GRAPH_RESULT, 0},
                                         {"bodies", "constraintBodies", GRAPH_RESULT, 0},
                                         {"contactImpulse", "constraintContactImpulse", GRAPH_RESULT, 0},
                                         {"jointImpulse", "constraintJointImpulse", GRAPH_RESULT, 0},
                                         {"resolve", "resolveJoint", JOINT, 0},
                                         {"point", "contactPoint", CONTACTS, 0},
                                         {"normal", "contactNormal", CONTACTS, 0},
                                         {"penetration", "contactPenetration", CONTACTS, 0},
                                         {"resolve", "resolveContacts", CONTACTS, 0},
                                         {"resolveSingle", "resolveSingleContact", CONTACTS, 0},
                                         {"impulse", "contactImpulse", RESULT, 0},
                                         {"setState", "bodySetState", BODY, PS_LANG_METHOD_MUTATING},
                                         {"applyImpulse", "bodyApplyImpulse", BODY, PS_LANG_METHOD_MUTATING},
                                         {"step", "bodyStep", BODY, PS_LANG_METHOD_MUTATING},
                                         {"kineticEnergy", "bodyKineticEnergy", BODY, 0},
                                         {"pointVelocity", "bodyPointVelocity", BODY, 0},
                                         {"forceTorque", "bodyForceTorque", BODY, 0},
                                         {"export", "exportTable", TABLE, 0},
                                         {"mean", "distributionMean", DIST, 0},
                                         {"standardDeviation", "distributionDeviation", DIST, 0},
                                         {"read", "sensorRead", SENSOR, PS_LANG_METHOD_MUTATING},
                                         {"reset", "sensorReset", SENSOR, PS_LANG_METHOD_MUTATING},
                                         {"resetForRun", "sensorResetForRun", SENSOR, PS_LANG_METHOD_MUTATING},
                                         {"nextTime", "sensorNextTime", SENSOR, 0},
                                         {"isValid", "measurementValid", SAMPLE, 0},
                                         {"isDue", "measurementDue", SAMPLE, 0},
                                         {"isDropped", "measurementDropped", SAMPLE, 0},
                                         {"dot", "dot2", V2, 0},
                                         {"dot", "dot3", V3, 0},
                                         {"dot", "dot4", V4, 0},
                                         {"cross", "cross2", V2, 0},
                                         {"cross", "cross3", V3, 0},
                                         {"length", "length2", V2, 0},
                                         {"length", "length3", V3, 0},
                                         {"length", "length4", V4, 0},
                                         {"normalized", "normalize2", V2, 0},
                                         {"normalized", "normalize3", V3, 0},
                                         {"normalized", "normalize4", V4, 0},
                                         {"normalized", "normalizeQuat", Q, 0},
                                         {"conjugated", "conjugateQuat", Q, 0},
                                         {"multiplied", "multiplyQuat", Q, 0},
                                         {"slerp", "slerpQuat", Q, 0},
                                         {"rotate", "rotate", Q, 0},
                                         {"symplectic", "symplectic", V2, 0},
                                         {"convert", "convert", U, 1},
                                         {"converted", "convertQuantity", QUANTITY, 0},
                                         {"adding", "addQuantity", QUANTITY, 0},
                                         {"subtracting", "subtractQuantity", QUANTITY, 0},
                                         {"multiplied", "multiplyQuantity", QUANTITY, 0},
                                         {"divided", "divideQuantity", QUANTITY, 0},
                                         {"multiplied", "multiplyUnit", U, 0},
                                         {"divided", "divideUnit", U, 0},
                                         {"powered", "powerUnit", U, 0},
                                         {"isCompatible", "compatibleUnit", U, 0},
                                         {"sample", "sample", C, 0},
                                         {"sampleCount", "datasetSampleCount", D, 0},
                                         {"channelCount", "datasetChannelCount", D, 0},
                                         {"channelName", "datasetChannelName", D, 0},
                                         {"channelUnitSymbol", "datasetChannelUnitSymbol", D, 0},
                                         {"channelDescription", "datasetChannelDescription", D, 0},
                                         {"channelExponent", "datasetChannelExponent", D, 0},
                                         {"recovered", "datasetRecovered", D, 0},
                                         {"metadata", "datasetMetadata", D, 0},
                                         {"series", "series", D, 0},
                                         {"close", "closeDataset", D, 0},
                                         {"count", "seriesCount", R, 0},
                                         {"masked","maskSeries",R,0},
                                         {"validity","seriesValidity",R,0},
                                         {"hasMask","seriesHasMask",R,0},
                                         {"isValid","seriesIsValid",R,0},
                                         {"name", "seriesName", R, 0},
                                         {"unitSymbol", "seriesUnitSymbol", R, 0},
                                         {"unitScale", "seriesUnitScale", R, 0},
                                         {"exponent", "seriesExponent", R, 0},
                                         {"isAlignedWith", "seriesAligned", R, 0},
                                         {"slice", "sliceSeries", R, 0},
                                         {"value", "seriesValue", R, 0},
                                         {"values", "seriesValues", R, 0},
                                         {"mean", "mean", R, 0},
                                         {"stddev", "stddev", R, 0},
                                         {"quantile", "quantile", R, 0},
                                         {"minimum", "minimum", R, 0},
                                         {"maximum", "maximum", R, 0},
                                         {"derivative", "derivative", R, 0},
                                         {"integral", "integral", R, 0},
                                         {"affine", "affine", R, 0},
                                         {"adding", "addSeries", R, 0},
                                         {"subtracting", "subtractSeries", R, 0},
                                         {"multiplied", "multiplySeries", R, 0},
                                         {"divided", "divideSeries", R, 0},
                                         {"resampledLinear", "resampleLinear", R, 0},
                                         {"resampledNearest", "resampleNearest", R, 0},
                                         {"resampledPrevious", "resamplePrevious", R, 0},
                                         {"resampledPchip", "resamplePchip", R, 0},
                                         {"movingAverage", "movingAverage", R, 0},
                                         {"release", "release", R, 0},
                                         {"alignedValues", "seriesAlignedValues", R, 0},
                                         {"plot", "plot", R, 1},
                                         {"histogram", "histogram", R, 0},
                                         {"export", "exportSeries", R, 1},
                                         {"curve", "curve", P, 0},
                                         {"points", "points", P, 0},
                                         {"export", "exportPlot", P, 0}};
const ps_lang_method *ps_lang_method_find(ps_lang_type owner, const void *name, size_t length,
                                          size_t *binding) {
    for (size_t i = 0; i < sizeof methods / sizeof methods[0]; i++) {
        const ps_lang_method *method = &methods[i];
        if (method->owner == owner && strlen(method->name) == length &&
            !memcmp(method->name, name, length)) {
            if (library_find(method->function, strlen(method->function), binding))
                return method;
        }
    }
    return NULL;
}
static const struct { const char *owner, *name, *function; } factories[] = {
    {"Diagnostic","here","diagnosticHere"}, {"Diagnostic","empty","emptyDiagnostic"}, {"Diagnostic","decode","diagnosticDecoded"}, {"Diagnostic","load","loadDiagnostic"},
    {"Int64", "abs", "intAbs"},
    {"Int64", "min", "intMin"},
    {"Int64", "max", "intMax"},
    {"Int64", "clamp", "intClamp"},
    {"Submersion", "sphere", "sphereSubmersion"},
    {"Medium", "air", "mediumAir"},
    {"Medium", "water", "mediumWater"},
    {"Medium", "vacuum", "mediumVacuum"},
    {"Mat3", "identity", "identityMat3"},
    {"Mat4", "identity", "identityMat4"},
    {"Mat4", "translation", "translationMat4"},
    {"Mat4", "scale", "scaleMat4"},
    {"Mat4", "rotation", "rotationMat4"},
    {"Mat4", "trs", "trsMat4"},
    {"Sweep", "spheres", "sweepSpheres"},
    {"Sweep", "spherePlane", "sweepSpherePlane"},
    {"Aabb", "sphere", "sphereBounds"},
    {"Aabb", "box", "boxBounds"},
    {"Aabb", "sweptSphere", "sweptSphereBounds"},
    {"Aabb", "pairs", "collisionPairs"},
    {"ContactSolver", "defaults", "defaultContactSolver"},
    {"Batch", "resume", "batchResume"},
    {"ContactWorld", "defaults", "defaultContactWorld"},
    {"Collider", "sphere", "sphereCollider"},
    {"Collider", "box", "boxCollider"},
    {"Collider", "plane", "planeCollider"},
    {"Contacts", "spheres", "sphereContacts"},
    {"Contacts", "spherePlane", "spherePlaneContacts"},
    {"Contacts", "sphereBox", "sphereBoxContacts"},
    {"Contacts", "boxPlane", "boxPlaneContacts"},
    {"Contacts", "boxes", "boxContacts"},
    {"Body", "sphere", "sphereBody"},
    {"Body", "box", "boxBody"},
    {"Series", "select", "selectSeries"},
    {"Series", "exportColumns", "exportColumns"},
    {"Series", "fromValues", "seriesFromValues"},
    {"Quat", "axisAngle", "axisAngle"},
    {"Distribution", "constant", "constantDistribution"},
    {"Distribution", "uniform", "uniformDistribution"},
    {"Distribution", "normal", "normalDistribution"},
    {"Sensor", "forRun", "sensorForRun"},
    {"Rng", "forRun", "rngForRun"}};
const ps_lang_builtin *ps_lang_builtin_find(const void *name, size_t length, size_t *binding) {
    for (size_t i = 0; i < sizeof factories / sizeof factories[0]; i++)
        if (strlen(factories[i].function) == length && !memcmp(name, factories[i].function, length))
            return NULL;
    for (size_t i = 0; i < sizeof methods / sizeof methods[0]; i++)
        if (strlen(methods[i].function) == length && !memcmp(methods[i].function, name, length))
            return NULL;
    return library_find(name, length, binding);
}
const ps_lang_builtin *ps_lang_static_method_find(const void *type, size_t type_length,
                                                  const void *name, size_t name_length,
                                                  size_t *binding) {
    for (size_t i = 0; i < sizeof factories / sizeof factories[0]; i++)
        if (strlen(factories[i].owner) == type_length &&
            !memcmp(type, factories[i].owner, type_length) &&
            strlen(factories[i].name) == name_length &&
            !memcmp(name, factories[i].name, name_length))
            return library_find(factories[i].function, strlen(factories[i].function), binding);
    return NULL;
}
const char *ps_lang_member_name(size_t binding) {
    if(binding==PS_LANG_MEMBER_STEP_ELAPSED) return "elapsed_s";
    if(binding==PS_LANG_MEMBER_STEP_NEXT) return "next_s";
    if (binding == PS_LANG_MEMBER_SUBMERSION_VOLUME) return "volume_m3";
    if (binding == PS_LANG_MEMBER_SUBMERSION_CENTROID) return "centroid_offset_m";
    if (binding == PS_LANG_MEMBER_MATERIAL_DENSITY) return "density_kg_m3";
    if (binding == PS_LANG_MEMBER_MATERIAL_RESTITUTION) return "restitution";
    if (binding == PS_LANG_MEMBER_MATERIAL_FRICTION) return "friction";
    if (binding == PS_LANG_MEMBER_MEDIUM_DENSITY) return "density_kg_m3";
    if (binding == PS_LANG_MEMBER_MEDIUM_VISCOSITY) return "viscosity_pa_s";
    if (binding == PS_LANG_MEMBER_ODE_STATE) return "state";
    if (binding == PS_LANG_MEMBER_ODE_ACCEPTED) return "accepted_steps";
    if (binding == PS_LANG_MEMBER_ODE_REJECTED) return "rejected_steps";
    if (binding == PS_LANG_MEMBER_ODE_EVALUATIONS) return "evaluations";
    if (binding == PS_LANG_MEMBER_ODE_REACHED_TIME) return "reached_time";
    if (binding == PS_LANG_MEMBER_ODE_NEXT_STEP) return "next_step";
    if (binding == PS_LANG_MEMBER_ODE_ERROR_NORM) return "error_norm";
    if (binding == PS_LANG_MEMBER_SCALAR_X) return "x";
    if (binding == PS_LANG_MEMBER_SCALAR_VALUE) return "value";
    if (binding == PS_LANG_MEMBER_SCALAR_LOWER) return "lower";
    if (binding == PS_LANG_MEMBER_SCALAR_UPPER) return "upper";
    if (binding == PS_LANG_MEMBER_SCALAR_ITERATIONS) return "iterations";
    if (binding == PS_LANG_MEMBER_SCALAR_EVALUATIONS) return "evaluations";
    if (binding == PS_LANG_MEMBER_SWEEP_HIT) return "hit";
    if (binding == PS_LANG_MEMBER_AABB_MIN) return "minimum_m";
    if (binding == PS_LANG_MEMBER_AABB_MAX) return "maximum_m";
    if (binding == PS_LANG_MEMBER_PAIR_A) return "a";
    if (binding == PS_LANG_MEMBER_PAIR_B) return "b";
    if (binding == PS_LANG_MEMBER_CONSTRAINT_A)
        return "body_a";
    if (binding == PS_LANG_MEMBER_CONSTRAINT_B)
        return "body_b";
    if (binding == PS_LANG_MEMBER_CONSTRAINT_POINT)
        return "contact.point_m";
    if (binding == PS_LANG_MEMBER_CONSTRAINT_NORMAL)
        return "contact.normal";
    if (binding == PS_LANG_MEMBER_CONSTRAINT_DEPTH)
        return "contact.penetration_m";
    if (binding == PS_LANG_MEMBER_CONSTRAINT_JOINT)
        return "joint";
    if (binding == PS_LANG_MEMBER_GRAPH_BODY_COUNT)
        return "body_count";
    if (binding == PS_LANG_MEMBER_GRAPH_CONTACT_COUNT)
        return "contact_count";
    if (binding == PS_LANG_MEMBER_GRAPH_JOINT_COUNT)
        return "joint_count";
    if (binding == PS_LANG_MEMBER_GRAPH_NORMAL_ERROR)
        return "normal_error";
    if (binding == PS_LANG_MEMBER_GRAPH_PROJECTION_ERROR)
        return "projection_error";
    if (binding == PS_LANG_MEMBER_GRAPH_JOINT_VELOCITY_ERROR)
        return "joint_velocity_error";
    if (binding == PS_LANG_MEMBER_GRAPH_JOINT_LENGTH_ERROR)
        return "joint_length_error";
    if (binding == PS_LANG_MEMBER_JOINT_ANCHOR_A)
        return "anchor_a_m";
    if (binding == PS_LANG_MEMBER_JOINT_ANCHOR_B)
        return "anchor_b_m";
    if (binding == PS_LANG_MEMBER_JOINT_LENGTH)
        return "length_m";
    if (binding == PS_LANG_MEMBER_JOINT_STABILIZATION)
        return "stabilization";
    if (binding == PS_LANG_MEMBER_JOINT_IMPULSE)
        return "solution.impulse_on_a_ns";
    if (binding == PS_LANG_MEMBER_JOINT_LENGTH_ERROR)
        return "solution.length_error_m";
    if (binding == PS_LANG_MEMBER_JOINT_VELOCITY_ERROR)
        return "solution.velocity_error_m_s";
    if (binding == PS_LANG_MEMBER_CONTACT_COUNT)
        return "count";
    if (binding == PS_LANG_MEMBER_RESULT_A)
        return "body_a";
    if (binding == PS_LANG_MEMBER_RESULT_B)
        return "body_b";
    if (binding == PS_LANG_MEMBER_RESULT_COUNT)
        return "solution.count";
    if (binding == PS_LANG_MEMBER_RESULT_ERROR)
        return "solution.max_normal_error_m_s";
    if (binding == PS_LANG_MEMBER_SOLVER_ITERATIONS)
        return "iterations";
    if (binding == PS_LANG_MEMBER_SOLVER_RESTITUTION)
        return "restitution";
    if (binding == PS_LANG_MEMBER_SOLVER_FRICTION)
        return "friction";
    if (binding == PS_LANG_MEMBER_SOLVER_THRESHOLD)
        return "bounce_threshold_m_s";
    if (binding == PS_LANG_MEMBER_SOLVER_SLOP)
        return "penetration_slop_m";
    if (binding == PS_LANG_MEMBER_SOLVER_CORRECTION)
        return "correction_fraction";
    if (binding == PS_LANG_MEMBER_BODY_POSITION)
        return "position_m";
    if (binding == PS_LANG_MEMBER_BODY_VELOCITY)
        return "velocity_m_s";
    if (binding == PS_LANG_MEMBER_BODY_ORIENTATION)
        return "orientation";
    if (binding == PS_LANG_MEMBER_BODY_ANGULAR_VELOCITY)
        return "angular_velocity_rad_s";
    if (binding == PS_LANG_MEMBER_BODY_MASS)
        return "mass_kg";
    if (binding == PS_LANG_MEMBER_BODY_INERTIA)
        return "inertia_kg_m2";
    if (binding == PS_LANG_MEMBER_MEASUREMENT_VALUE)
        return "value";
    if (binding == PS_LANG_MEMBER_STATE)
        return "state";
    if (binding == PS_LANG_MEMBER_TIME)
        return "time_s";
    if (binding == PS_LANG_MEMBER_UNCERTAINTY)
        return "standard_uncertainty";
    if (binding == PS_LANG_MEMBER_INDEX)
        return "index";
    if (binding == PS_LANG_MEMBER_SKIPPED)
        return "skipped";
    if (binding == PS_LANG_MEMBER_VALUE)
        return "value";
    if (binding == PS_LANG_MEMBER_UNIT)
        return "unit";
    return binding == PS_LANG_MEMBER_X   ? "x"
           : binding == PS_LANG_MEMBER_Y ? "y"
           : binding == PS_LANG_MEMBER_Z ? "z"
           : binding == PS_LANG_MEMBER_W ? "w"
                                         : NULL;
}
