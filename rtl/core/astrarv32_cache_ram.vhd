-- ================================================================================ --
-- ASTRARV32 SoC - Generic Cache - Tag and Data RAM Primitive Wrapper                 --
-- -------------------------------------------------------------------------------- --
-- Replace this file by a more efficient technology-specific IP wrapper. The read-  --
-- during-write behavior is irrelevant as read/write accesses are mutual exclusive. --
-- -------------------------------------------------------------------------------- --
-- The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              --
-- Copyright (c) ASTRARV32 contributors.                                              --
-- Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  --
-- Licensed under the BSD-3-Clause license, see LICENSE for details.                --
-- SPDX-License-Identifier: BSD-3-Clause                                            --
-- ================================================================================ --

library ieee;
use ieee.std_logic_1164.all;

library astrarv32;
use astrarv32.astrarv32_package.all;

entity astrarv32_cache_ram is
  generic (
    TAG_WIDTH : natural; -- tag width
    IDX_WIDTH : natural; -- index width
    OFS_WIDTH : natural  -- offset width
  );
  port (
    -- global control --
    clk_i     : in  std_ulogic;                     -- global clock, rising edge
    addr_i    : in  std_ulogic_vector(31 downto 0); -- full byte address
    -- tag memory --
    tag_we_i  : in  std_ulogic;                     -- tag write-enable
    tag_o     : out std_ulogic_vector(31 downto 0); -- zero-extended tag output
    -- data memory --
    data_we_i : in  std_ulogic_vector(3 downto 0);  -- byte-wise data write-enable
    data_i    : in  std_ulogic_vector(31 downto 0); -- write data
    data_o    : out std_ulogic_vector(31 downto 0)  -- read data
  );
end entity;

architecture astrarv32_cache_ram_rtl of astrarv32_cache_ram is

  signal tag_rd : std_ulogic_vector(TAG_WIDTH-1 downto 0);

begin

  -- Tag RAM --------------------------------------------------------------------------------
  -- -------------------------------------------------------------------------------------------
  tag_ram_inst: entity astrarv32.astrarv32_prim_spram
  generic map (
    AWIDTH => IDX_WIDTH,
    DWIDTH => TAG_WIDTH,
    OUTREG => false
  )
  port map (
    clk_i  => clk_i,
    en_i   => '1',
    rw_i   => tag_we_i,
    addr_i => addr_i(31-TAG_WIDTH downto 2+OFS_WIDTH), -- index
    data_i => addr_i(31 downto 31-(TAG_WIDTH-1)), -- tag
    data_o => tag_rd
  );

  tag_o(TAG_WIDTH-1 downto 0) <= tag_rd(TAG_WIDTH-1 downto 0); -- actual tag
  tag_o(31 downto TAG_WIDTH)  <= (others => '0'); -- zero-extend

  -- Data RAM -------------------------------------------------------------------------------
  -- -------------------------------------------------------------------------------------------
  data_ram_gen:
  for i in 0 to 3 generate -- four individual byte RAMs per word
    data_ram_inst: entity astrarv32.astrarv32_prim_spram
    generic map (
      AWIDTH => IDX_WIDTH + OFS_WIDTH,
      DWIDTH => 8,
      OUTREG => false
    )
    port map (
      clk_i  => clk_i,
      en_i   => '1',
      rw_i   => data_we_i(i),
      addr_i => addr_i(IDX_WIDTH+OFS_WIDTH+1 downto 2), -- index & word-offset
      data_i => data_i(i*8+7 downto i*8),
      data_o => data_o(i*8+7 downto i*8)
    );
  end generate;

end architecture;
