# Legacy finding keys

Findings once carried `FPGA-nnn` keys borrowed from the private workspace where
this work is planned. That workspace is not published, so no reader outside it
could resolve such a reference — a failing assertion printed a key that led
nowhere — and the borrowed numbers carried the gaps of a task sequence
(`050, 051, 060, 062, 069`) that has nothing to do with findings. They were
renumbered `NM-HRM-nnn`.

This table is here so an older commit message or artefact can still be read. It
is not needed to use the register and does not belong in it.

| now | was | | now | was | | now | was |
|---|---|---|---|---|---|---|---|
| NM-HRM-001 | FPGA-050 | | NM-HRM-009 | FPGA-091-NM-HRM-009 | | NM-HRM-016 | FPGA-091-NM-HRM-016 |
| NM-HRM-002 | FPGA-051 | | NM-HRM-010 | FPGA-091-NM-HRM-010 | | NM-HRM-017 | FPGA-091-NM-HRM-017 |
| NM-HRM-003 | FPGA-060 | | NM-HRM-011 | FPGA-091-NM-HRM-011 | | NM-HRM-018 | FPGA-091-NM-HRM-018 |
| NM-HRM-004 | FPGA-062 | | NM-HRM-012 | FPGA-091-NM-HRM-012 | | NM-HRM-019 | FPGA-091-NM-HRM-019 |
| NM-HRM-005 | FPGA-069 | | NM-HRM-013 | FPGA-091-NM-HRM-013 | | NM-HRM-020 | *newly issued* |
| NM-HRM-006 | FPGA-091-NM-HRM-006 | | NM-HRM-014 | FPGA-091-NM-HRM-014 | | NM-HRM-021 | *newly issued* |
| NM-HRM-007 | FPGA-091-NM-HRM-007 | | NM-HRM-015 | FPGA-091-NM-HRM-015 | | NM-HRM-022 | *newly issued* |
| NM-HRM-008 | FPGA-091-NM-HRM-008 | | NM-HRM-037 | *newly issued* | | NM-HRM-039 | *newly issued* |
| | | | NM-HRM-038 | *newly issued* | | NM-HRM-040 | *newly issued* |

**A second, unrelated use of `FPGA-nnn` is still live** and is not covered by
this table: in a test source, `// Primary oracle: Frozen FPGA-019 specification`
marks a package whose oracle is a frozen module specification rather than the
manual, and therefore **not** an HRM-conformance test. Those references stay.
