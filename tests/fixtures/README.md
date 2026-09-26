# CRC compatibility fixture

`crc-report-v1.bin` is an unmodified `.psreport` v1 produced before the CRC lookup
table change on 2026-09-19. It contains the bundled pendulum example's analysis of
1,025 samples (RK4, dt 0.005 s, seed 42), including its original provenance text.
The `.bin` suffix keeps it outside the generated-run ignore rule.

SHA-256: `3fd9545e3c8ecf05dd7f71adb369d668094747ebc1b9d941856357a8dd3e9caa`.

`crc_report_roundtrip` loads it with the current reader, writes it through the
current serializer to an exclusive temporary filename and compares every byte.
Do not regenerate this fixture with the implementation it is intended to check.

`../crc_vectors.h` contains separate frozen values from Python's `zlib.crc32` for
the byte pattern `(i*31) ^ (i>>8) ^ 0xa5`, truncated to one byte, at the listed
lengths. These are independent of Physim's lookup-table generator. The C test also
retains a bit-at-a-time reference, tests unaligned starts and flips every bit of
a 136-byte sample payload.
