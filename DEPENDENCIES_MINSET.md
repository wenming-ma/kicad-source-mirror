# KiCad PCB最小可编译依赖集合分析报告

**基于BOARD和FOOTPRINT核心数据的编译期依赖分析结果**

## 📊 统计摘要

- **最小源文件集合**: 288 个 .cpp/.cc 文件
- **最小头文件集合**: 2113 个 .h/.hpp 文件
- **未解析符号**: 6641 个 (需要外部库)

## 🔧 最小源文件集合 (.cpp/.cc)

以下源文件构成了BOARD/FOOTPRINT功能的最小编译单元：

  1. `bitmap2component\bitmap2cmp_control.cpp`
  2. `bitmap2component\bitmap2cmp_frame.cpp`
  3. `bitmap2component\bitmap2cmp_main.cpp`
  4. `bitmap2component\bitmap2cmp_panel.cpp`
  5. `bitmap2component\bitmap2cmp_panel_base.cpp`
  6. `bitmap2component\bitmap2cmp_settings.cpp`
  7. `bitmap2component\bitmap2component.cpp`
  8. `build\api\cpp\api\common\envelope.pb.cc`
  9. `build\common\drawing_sheet\drawing_sheet_keywords.cpp`
 10. `build\common\drc_rules_keywords.cpp`
 11. `build\common\embedded_files_keywords.cpp`
 12. `build\common\lib_table_keywords.cpp`
 13. `build\common\pcb_plot_params_keywords.cpp`
 14. `common\advanced_config.cpp`
 15. `common\api\api_enums.cpp`
 16. `common\api\api_handler.cpp`
 17. `common\api\api_plugin.cpp`
 18. `common\api\api_plugin_manager.cpp`
 19. `common\api\api_server.cpp`
 20. `common\api\api_utils.cpp`
 21. `common\api\serializable.cpp`
 22. `common\asset_archive.cpp`
 23. `common\background_jobs_monitor.cpp`
 24. `common\bin_mod.cpp`
 25. `common\bitmap.cpp`
 26. `common\bitmap_base.cpp`
 27. `common\bitmap_info.cpp`
 28. `common\bitmap_store.cpp`
 29. `common\build_version.cpp`
 30. `common\common.cpp`
 31. `common\config_params.cpp`
 32. `common\confirm.cpp`
 33. `common\database\database_lib_settings.cpp`
 34. `common\design_block_info.cpp`
 35. `common\design_block_info_impl.cpp`
 36. `common\design_block_io.cpp`
 37. `common\design_block_lib_table.cpp`
 38. `common\dialog_about\AboutDialog_main.cpp`
 39. `common\dialog_about\dialog_about.cpp`
 40. `common\dialog_about\dialog_about_base.cpp`
 41. `common\dialog_shim.cpp`
 42. `common\dialogs\dialog_color_picker.cpp`
 43. `common\dialogs\dialog_color_picker_base.cpp`
 44. `common\dialogs\dialog_configure_paths.cpp`
 45. `common\dialogs\dialog_configure_paths_base.cpp`
 46. `common\dialogs\dialog_hotkey_list.cpp`
 47. `common\dialogs\dialog_migrate_settings.cpp`
 48. `common\dialogs\dialog_migrate_settings_base.cpp`
 49. `common\dialogs\git\panel_git_repos.cpp`
 50. `common\dialogs\git\panel_git_repos_base.cpp`
 51. `common\dialogs\panel_common_settings.cpp`
 52. `common\dialogs\panel_common_settings_base.cpp`
 53. `common\dialogs\panel_hotkeys_editor.cpp`
 54. `common\dialogs\panel_mouse_settings.cpp`
 55. `common\dialogs\panel_mouse_settings_base.cpp`
 56. `common\dialogs\panel_packages_and_updates.cpp`
 57. `common\dialogs\panel_packages_and_updates_base.cpp`
 58. `common\dialogs\panel_plugin_settings.cpp`
 59. `common\dialogs\panel_plugin_settings_base.cpp`
 60. `common\dsnlexer.cpp`
 61. `common\eda_base_frame.cpp`
 62. `common\eda_dde.cpp`
 63. `common\eda_doc.cpp`
 64. `common\eda_item.cpp`
 65. `common\eda_pattern_match.cpp`
 66. `common\eda_shape.cpp`
 67. `common\eda_text.cpp`
 68. `common\eda_units.cpp`
 69. `common\embedded_files.cpp`
 70. `common\env_paths.cpp`
 71. `common\env_vars.cpp`
 72. `common\exceptions.cpp`
 73. `common\file_history.cpp`
 74. `common\font\font.cpp`
 75. `common\font\fontconfig.cpp`
 76. `common\font\glyph.cpp`
 77. `common\font\outline_decomposer.cpp`
 78. `common\font\outline_font.cpp`
 79. `common\font\stroke_font.cpp`
 80. `common\font\text_attributes.cpp`
 81. `common\font\version_info.cpp`
 82. `common\gal\color4d.cpp`
 83. `common\gal\graphics_abstraction_layer.cpp`
 84. `common\gal\opengl\gl_context_mgr.cpp`
 85. `common\gal\painter.cpp`
 86. `common\gestfich.cpp`
 87. `common\gr_basic.cpp`
 88. `common\gr_text.cpp`
 89. `common\grid_tricks.cpp`
 90. `common\hotkey_store.cpp`
 91. `common\hotkeys_basic.cpp`
 92. `common\io\io_base.cpp`
 93. `common\io\kicad\kicad_io_utils.cpp`
 94. `common\json_conversions.cpp`
 95. `common\json_schema_validator.cpp`
 96. `common\kicad_curl\kicad_curl.cpp`
 97. `common\kiface_base.cpp`
 98. `common\kiid.cpp`
 99. `common\kiway.cpp`
100. `common\kiway_express.cpp`
101. `common\kiway_holder.cpp`
102. `common\kiway_player.cpp`
103. `common\launch_ext.cpp`
104. `common\layer_id.cpp`
105. `common\lib_id.cpp`
106. `common\lib_table_base.cpp`
107. `common\libeval_compiler\libeval_compiler.cpp`
108. `common\locale_io.cpp`
109. `common\lset.cpp`
110. `common\marker_base.cpp`
111. `common\netclass.cpp`
112. `common\notifications_manager.cpp`
113. `common\origin_transforms.cpp`
114. `common\paths.cpp`
115. `common\pgm_base.cpp`
116. `common\project.cpp`
117. `common\project\board_project_settings.cpp`
118. `common\project\net_settings.cpp`
119. `common\project\project_archiver.cpp`
120. `common\project\project_file.cpp`
121. `common\project\project_local_settings.cpp`
122. `common\properties\property_mgr.cpp`
123. `common\rc_item.cpp`
124. `common\refdes_utils.cpp`
125. `common\reference_image.cpp`
126. `common\render_settings.cpp`
127. `common\reporter.cpp`
128. `common\richio.cpp`
129. `common\scintilla_tricks.cpp`
130. `common\search_stack.cpp`
131. `common\settings\app_settings.cpp`
132. `common\settings\aui_settings.cpp`
133. `common\settings\bom_settings.cpp`
134. `common\settings\color_settings.cpp`
135. `common\settings\common_settings.cpp`
136. `common\settings\grid_settings.cpp`
137. `common\settings\json_settings.cpp`
138. `common\settings\kicad_settings.cpp`
139. `common\settings\layer_settings_utils.cpp`
140. `common\settings\nested_settings.cpp`
141. `common\settings\parameters.cpp`
142. `common\settings\settings_manager.cpp`
143. `common\singleton.cpp`
144. `common\string_utils.cpp`
145. `common\stroke_params.cpp`
146. `common\systemdirsappend.cpp`
147. `common\template_fieldnames.cpp`
148. `common\textentry_tricks.cpp`
149. `common\thread_pool.cpp`
150. `common\title_block.cpp`
151. `common\tool\action_manager.cpp`
152. `common\tool\action_menu.cpp`
153. `common\tool\action_toolbar.cpp`
154. `common\tool\actions.cpp`
155. `common\tool\common_control.cpp`
156. `common\tool\conditional_menu.cpp`
157. `common\tool\selection.cpp`
158. `common\tool\tool_action.cpp`
159. `common\tool\tool_base.cpp`
160. `common\tool\tool_dispatcher.cpp`
161. `common\tool\tool_event.cpp`
162. `common\tool\tool_interactive.cpp`
163. `common\tool\tool_manager.cpp`
164. `common\tool\tool_menu.cpp`
165. `common\tool\tools_holder.cpp`
166. `common\trace_helpers.cpp`
167. `common\ui_events.cpp`
168. `common\undo_redo_container.cpp`
169. `common\validators.cpp`
170. `common\view\view.cpp`
171. `common\view\view_controls.cpp`
172. `common\view\view_group.cpp`
173. `common\view\view_item.cpp`
174. `common\view\view_overlay.cpp`
175. `common\view\wx_view_controls.cpp`
176. `common\view\zoom_controller.cpp`
177. `common\widgets\bitmap_button.cpp`
178. `common\widgets\button_row_panel.cpp`
179. `common\widgets\color_swatch.cpp`
180. `common\widgets\grid_text_button_helpers.cpp`
181. `common\widgets\grid_text_helpers.cpp`
182. `common\widgets\html_window.cpp`
183. `common\widgets\kistatusbar.cpp`
184. `common\widgets\paged_dialog.cpp`
185. `common\widgets\progress_reporter_base.cpp`
186. `common\widgets\std_bitmap_button.cpp`
187. `common\widgets\ui_common.cpp`
188. `common\widgets\widget_hotkey_list.cpp`
189. `common\widgets\wx_aui_art_providers.cpp`
190. `common\widgets\wx_busy_indicator.cpp`
191. `common\widgets\wx_grid.cpp`
192. `common\widgets\wx_grid_autosizer.cpp`
193. `common\widgets\wx_infobar.cpp`
194. `common\widgets\wx_panel.cpp`
195. `common\widgets\wx_treebook.cpp`
196. `common\wildcards_and_files_ext.cpp`
197. `libs\core\observable.cpp`
198. `libs\core\utf8.cpp`
199. `libs\core\wx_stl_compat.cpp`
200. `libs\kimath\src\bezier_curves.cpp`
201. `libs\kimath\src\convert_basic_shapes_to_polygon.cpp`
202. `libs\kimath\src\geometry\circle.cpp`
203. `libs\kimath\src\geometry\convex_hull.cpp`
204. `libs\kimath\src\geometry\eda_angle.cpp`
205. `libs\kimath\src\geometry\geometry_utils.cpp`
206. `libs\kimath\src\geometry\half_line.cpp`
207. `libs\kimath\src\geometry\line.cpp`
208. `libs\kimath\src\geometry\seg.cpp`
209. `libs\kimath\src\geometry\shape.cpp`
210. `libs\kimath\src\geometry\shape_arc.cpp`
211. `libs\kimath\src\geometry\shape_collisions.cpp`
212. `libs\kimath\src\geometry\shape_compound.cpp`
213. `libs\kimath\src\geometry\shape_line_chain.cpp`
214. `libs\kimath\src\geometry\shape_poly_set.cpp`
215. `libs\kimath\src\geometry\shape_rect.cpp`
216. `libs\kimath\src\geometry\shape_segment.cpp`
217. `libs\kimath\src\geometry\shape_utils.cpp`
218. `libs\kimath\src\geometry\vertex_set.cpp`
219. `libs\kimath\src\math\util.cpp`
220. `libs\kimath\src\math\vector2.cpp`
221. `libs\kimath\src\trigo.cpp`
222. `libs\kinng\src\kinng.cpp`
223. `libs\kiplatform\os\windows\app.cpp`
224. `libs\kiplatform\os\windows\environment.cpp`
225. `libs\kiplatform\os\windows\io.cpp`
226. `libs\kiplatform\port\wxmsw\ui.cpp`
227. `pcbnew\api\api_pcb_enums.cpp`
228. `pcbnew\api\api_pcb_utils.cpp`
229. `pcbnew\board.cpp`
230. `pcbnew\board_connected_item.cpp`
231. `pcbnew\board_design_settings.cpp`
232. `pcbnew\board_item.cpp`
233. `pcbnew\board_stackup_manager\board_stackup.cpp`
234. `pcbnew\collectors.cpp`
235. `pcbnew\component_class_manager.cpp`
236. `pcbnew\connectivity\connectivity_algo.cpp`
237. `pcbnew\connectivity\connectivity_data.cpp`
238. `pcbnew\connectivity\connectivity_items.cpp`
239. `pcbnew\connectivity\from_to_cache.cpp`
240. `pcbnew\convert_shape_list_to_polygon.cpp`
241. `pcbnew\drc\drc_cache_generator.cpp`
242. `pcbnew\drc\drc_engine.cpp`
243. `pcbnew\drc\drc_item.cpp`
244. `pcbnew\drc\drc_rule.cpp`
245. `pcbnew\drc\drc_rule_condition.cpp`
246. `pcbnew\drc\drc_rule_parser.cpp`
247. `pcbnew\drc\drc_test_provider.cpp`
248. `pcbnew\footprint.cpp`
249. `pcbnew\netinfo_item.cpp`
250. `pcbnew\netinfo_list.cpp`
251. `pcbnew\pad.cpp`
252. `pcbnew\pad_utils.cpp`
253. `pcbnew\padstack.cpp`
254. `pcbnew\pcb_dimension.cpp`
255. `pcbnew\pcb_field.cpp`
256. `pcbnew\pcb_generator.cpp`
257. `pcbnew\pcb_group.cpp`
258. `pcbnew\pcb_marker.cpp`
259. `pcbnew\pcb_plot_params.cpp`
260. `pcbnew\pcb_reference_image.cpp`
261. `pcbnew\pcb_shape.cpp`
262. `pcbnew\pcb_table.cpp`
263. `pcbnew\pcb_tablecell.cpp`
264. `pcbnew\pcb_text.cpp`
265. `pcbnew\pcb_textbox.cpp`
266. `pcbnew\pcb_track.cpp`
267. `pcbnew\pcbexpr_evaluator.cpp`
268. `pcbnew\pcbexpr_functions.cpp`
269. `pcbnew\ratsnest\ratsnest_data.cpp`
270. `pcbnew\router\pns_meander.cpp`
271. `pcbnew\teardrop\teardrop_parameters.cpp`
272. `pcbnew\zone.cpp`
273. `pcbnew\zone_settings.cpp`
274. `scripting\python_manager.cpp`
275. `scripting\python_scripting.cpp`
276. `thirdparty\clipper2\Clipper2Lib\src\clipper.engine.cpp`
277. `thirdparty\clipper2\Clipper2Lib\src\clipper.offset.cpp`
278. `thirdparty\delaunator\delaunator.cpp`
279. `thirdparty\fmt\src\format.cc`
280. `thirdparty\json_schema_validator\json-patch.cpp`
281. `thirdparty\json_schema_validator\json-uri.cpp`
282. `thirdparty\json_schema_validator\json-validator.cpp`
283. `thirdparty\libcontext\libcontext.cpp`
284. `thirdparty\other_math\math_for_graphics.cpp`
285. `thirdparty\potrace\src\curve.cpp`
286. `thirdparty\potrace\src\decompose.cpp`
287. `thirdparty\potrace\src\potracelib.cpp`
288. `thirdparty\potrace\src\trace.cpp`

## 📁 最小头文件闭包 (.h/.hpp)

以下头文件是上述源文件的完整依赖闭包：

### bitmap2component/

- `bitmap2component\bitmap2cmp_control.h`
- `bitmap2component\bitmap2cmp_frame.h`
- `bitmap2component\bitmap2cmp_panel.h`
- `bitmap2component\bitmap2cmp_panel_base.h`
- `bitmap2component\bitmap2cmp_settings.h`
- `bitmap2component\bitmap2component.h`

### build/

- `build\config.h`
- `build\kicad_build_version.h`

### build\api\cpp\api\board/

- `build\api\cpp\api\board\board.pb.h`
- `build\api\cpp\api\board\board_commands.pb.h`
- `build\api\cpp\api\board\board_types.pb.h`

### build\api\cpp\api\common/

- `build\api\cpp\api\common\envelope.pb.h`

### build\api\cpp\api\common\types/

- `build\api\cpp\api\common\types\base_types.pb.h`
- `build\api\cpp\api\common\types\enums.pb.h`
- `build\api\cpp\api\common\types\project_settings.pb.h`

### build\api\cpp\api\schematic/

- `build\api\cpp\api\schematic\schematic_types.pb.h`

### build\common/

- `build\common\drc_rules_lexer.h`
- `build\common\embedded_files_lexer.h`
- `build\common\lib_table_lexer.h`
- `build\common\pcb_plot_params_lexer.h`
- `build\common\stroke_params_lexer.h`
- `build\common\template_fieldnames_lexer.h`

### build\common\drawing_sheet/

- `build\common\drawing_sheet\drawing_sheet_lexer.h`

### build\common\libeval_compiler/

- `build\common\libeval_compiler\grammar.h`

### build\vcpkg_installed\x64-windows\debug\lib\mswud\wx/

- `build\vcpkg_installed\x64-windows\debug\lib\mswud\wx\setup.h`

### build\vcpkg_installed\x64-windows\include/

- `build\vcpkg_installed\x64-windows\include\ft2build.h`
- `build\vcpkg_installed\x64-windows\include\git2.h`
- `build\vcpkg_installed\x64-windows\include\zstd.h`
- `build\vcpkg_installed\x64-windows\include\zstd_errors.h`

### build\vcpkg_installed\x64-windows\include\GL/

- `build\vcpkg_installed\x64-windows\include\GL\gl.h`

### build\vcpkg_installed\x64-windows\include\boost/

- `build\vcpkg_installed\x64-windows\include\boost\assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\call_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\checked_delete.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\compressed_pair.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept_check.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\cstdint.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\limits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\next_prior.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\pointee.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\scoped_array.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\static_assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\throw_exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\version.hpp`

### build\vcpkg_installed\x64-windows\include\boost\algorithm\string/

- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\case_conv.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\config.hpp`

### build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail/

- `build\vcpkg_installed\x64-windows\include\boost\algorithm\string\detail\case_conv.hpp`

### build\vcpkg_installed\x64-windows\include\boost\assert/

- `build\vcpkg_installed\x64-windows\include\boost\assert\source_location.hpp`

### build\vcpkg_installed\x64-windows\include\boost\concept/

- `build\vcpkg_installed\x64-windows\include\boost\concept\assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\usage.hpp`

### build\vcpkg_installed\x64-windows\include\boost\concept\detail/

- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\backward_compatibility.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\concept_def.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\concept_undef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\has_constraints.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\concept\detail\msvc.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config/

- `build\vcpkg_installed\x64-windows\include\boost\config\helper_macros.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\pragma_message.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\user.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\compiler/

- `build\vcpkg_installed\x64-windows\include\boost\config\compiler\visualc.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\detail/

- `build\vcpkg_installed\x64-windows\include\boost\config\detail\cxx_composite.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\select_compiler_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\select_platform_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\select_stdlib_config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\config\detail\suffix.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\no_tr1/

- `build\vcpkg_installed\x64-windows\include\boost\config\no_tr1\cmath.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\platform/

- `build\vcpkg_installed\x64-windows\include\boost\config\platform\win32.hpp`

### build\vcpkg_installed\x64-windows\include\boost\config\stdlib/

- `build\vcpkg_installed\x64-windows\include\boost\config\stdlib\dinkumware.hpp`

### build\vcpkg_installed\x64-windows\include\boost\core/

- `build\vcpkg_installed\x64-windows\include\boost\core\addressof.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\bit.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\checked_delete.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\empty_value.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\enable_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\invoke_swap.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\noncopyable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\core\use_default.hpp`

### build\vcpkg_installed\x64-windows\include\boost\detail/

- `build\vcpkg_installed\x64-windows\include\boost\detail\call_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\detail\compressed_pair.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\detail\is_incrementable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\detail\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\exception/

- `build\vcpkg_installed\x64-windows\include\boost\exception\exception.hpp`

### build\vcpkg_installed\x64-windows\include\boost\integer/

- `build\vcpkg_installed\x64-windows\include\boost\integer\integer_log2.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer\integer_mask.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\integer\static_log2.hpp`

### build\vcpkg_installed\x64-windows\include\boost\iterator/

- `build\vcpkg_installed\x64-windows\include\boost\iterator\advance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\distance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\enable_if_convertible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\interoperable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\is_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_adaptor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_categories.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_concepts.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_facade.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\iterator_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\reverse_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\transform_iterator.hpp`

### build\vcpkg_installed\x64-windows\include\boost\iterator\detail/

- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\config_def.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\config_undef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\eval_if_default.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\facade_iterator_category.hpp`

### build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits/

- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\conjunction.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\disjunction.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\negation.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\iterator\detail\type_traits\type_identity.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mp11/

- `build\vcpkg_installed\x64-windows\include\boost\mp11\integral.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\utility.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\version.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mp11\detail/

- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_defer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_fold.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_front.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_list.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_rename.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mp11\detail\mp_value.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\and.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\arg_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\bool.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\bool_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\eval_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\has_xxx.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\int.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\int_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\integral_c_tag.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\lambda_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\not.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\or.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\void_fwd.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\adl_barrier.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\arity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\include_preprocessed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\integral_wrapper.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\lambda_arity_param.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\lambda_support.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\na_spec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\nested_type_wknd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\nttp_decl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\static_cast.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\template_arity_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\type_wrapper.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\value_wknd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\yes_no.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\adl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\arrays.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\compiler.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\ctps.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\dtp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\eti.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\gcc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\gpu.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\has_xxx.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\integral.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\intel.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\lambda.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\msvc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\msvc_typename.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\nttp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\overload_resolution.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\pp_counter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\preprocessor.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\static_constant.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\ttp.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\use_preprocessed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\config\workaround.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\and.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessed\plain\or.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\def_params_tail.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\mpl\aux_\preprocessor\params.hpp`

### build\vcpkg_installed\x64-windows\include\boost\mpl\limits/

- `build\vcpkg_installed\x64-windows\include\boost\mpl\limits\arity.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\cat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\comma_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\inc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repeat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\stringize.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\add.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\dec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\inc.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\mod.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\sub.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\arithmetic\detail\div_base.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\array/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\array\data.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\array\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\array\size.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\comparison/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\comparison\less_equal.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\config/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\config\config.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\control/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\deduce_d.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\expr_iif.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\iif.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\while.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\detail\msvc/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\control\detail\msvc\while.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\debug/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\debug\error.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail\auto_rec.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail\check.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\detail\is_binary.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\check_empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\expand.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\identity.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\is_1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\is_empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\is_empty_variadic.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\overload.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\facilities\detail\is_empty.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\list/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\adt.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\fold_left.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\fold_right.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\reverse.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\detail\fold_left.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\list\detail\fold_right.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\and.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\bitand.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\bool.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\compl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\logical\not.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\comma.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\comma_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\is_begin_parens.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\punctuation\detail\is_begin_parens.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum_binary_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\enum_trailing_params.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\for.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\repeat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\repeat_from_to.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\detail\msvc/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\repetition\detail\msvc\for.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\cat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\fold_left.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\for_each_i.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\seq.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\transform.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\seq\detail\is_empty.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\eat.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\rem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\size.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\tuple\detail\is_single_return.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\elem.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\has_opt.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\size.hpp`

### build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\detail/

- `build\vcpkg_installed\x64-windows\include\boost\preprocessor\variadic\detail\has_opt.hpp`

### build\vcpkg_installed\x64-windows\include\boost\ptr_container/

- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\clone_allocator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\indirect_fun.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\nullable.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_sequence_adapter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\ptr_vector.hpp`

### build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail/

- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\default_deleter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\is_convertible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\move.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\ptr_container_disable_deprecated.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\reversible_ptr_container.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\scoped_deleter.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\static_move_ptr.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\throw_exception.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\ptr_container\detail\void_ptr_iterator.hpp`

### build\vcpkg_installed\x64-windows\include\boost\random/

- `build\vcpkg_installed\x64-windows\include\boost\random\mersenne_twister.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\traits.hpp`

### build\vcpkg_installed\x64-windows\include\boost\random\detail/

- `build\vcpkg_installed\x64-windows\include\boost\random\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\const_mod.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\disable_warnings.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\enable_warnings.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\generator_bits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\generator_seed_seq.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\integer_log2.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\large_arithmetic.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\polynomial.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\ptr_helper.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\seed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\seed_impl.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\random\detail\signed_unsigned_tools.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range/

- `build\vcpkg_installed\x64-windows\include\boost\range\as_literal.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\begin.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\concepts.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\const_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\difference_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\distance.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\end.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\functions.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\has_range_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator_range.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator_range_core.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\iterator_range_io.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\mutable_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\range_fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\rbegin.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\rend.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\reverse_iterator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\size_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\value_type.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range\algorithm/

- `build\vcpkg_installed\x64-windows\include\boost\range\algorithm\equal.hpp`

### build\vcpkg_installed\x64-windows\include\boost\range\detail/

- `build\vcpkg_installed\x64-windows\include\boost\range\detail\common.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\extract_optional_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\has_member_size.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\implementation_help.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\misc_concept.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\msvc_has_iterator_workaround.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\safe_bool.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\sfinae.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\range\detail\str_types.hpp`

### build\vcpkg_installed\x64-windows\include\boost\smart_ptr/

- `build\vcpkg_installed\x64-windows\include\boost\smart_ptr\scoped_array.hpp`

### build\vcpkg_installed\x64-windows\include\boost\smart_ptr\detail/

- `build\vcpkg_installed\x64-windows\include\boost\smart_ptr\detail\deprecated_macros.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\smart_ptr\detail\sp_noexcept.hpp`

### build\vcpkg_installed\x64-windows\include\boost\type_traits/

- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_const.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_rvalue_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\add_volatile.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\conditional.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\conversion_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\declval.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\function_traits.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_minus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_minus_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_plus.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\has_plus_assign.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\integral_constant.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\intrinsics.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_abstract.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_arithmetic.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_array.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_base_and_derived.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_class.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_complete.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_const.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_convertible.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_empty.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_enum.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_final.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_floating_point.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_function.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_integral.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_lvalue_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_member_function_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_rvalue_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_same.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_signed.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_unsigned.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_void.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\is_volatile.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\make_unsigned.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\make_void.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_bounds.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_const.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_cv.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_extent.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_pointer.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\remove_reference.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\type_identity.hpp`

### build\vcpkg_installed\x64-windows\include\boost\type_traits\detail/

- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\bool_trait_undef.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\has_binary_operator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\is_function_cxx_11.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\is_member_function_pointer_cxx_11.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\type_traits\detail\yes_no_type.hpp`

### build\vcpkg_installed\x64-windows\include\boost\utility/

- `build\vcpkg_installed\x64-windows\include\boost\utility\base_from_member.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\binary.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\enable_if.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\identity_type.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\utility\result_of.hpp`

### build\vcpkg_installed\x64-windows\include\boost\utility\detail/

- `build\vcpkg_installed\x64-windows\include\boost\utility\detail\result_of_variadic.hpp`

### build\vcpkg_installed\x64-windows\include\boost\uuid/

- `build\vcpkg_installed\x64-windows\include\boost\uuid\basic_random_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\entropy_error.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\name_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\name_generator_md5.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\name_generator_sha1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\namespaces.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\nil_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\random_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\string_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator_v1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator_v6.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\time_generator_v7.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid_clock.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid_generators.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\uuid_io.hpp`

### build\vcpkg_installed\x64-windows\include\boost\uuid\detail/

- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\basic_name_generator.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\chacha20.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\config.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\endian.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\hash_mix.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\md5.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\numeric_cast.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\random_device.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\random_provider.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\sha1.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\static_assert.hpp`
- `build\vcpkg_installed\x64-windows\include\boost\uuid\detail\to_chars.hpp`

### build\vcpkg_installed\x64-windows\include\curl/

- `build\vcpkg_installed\x64-windows\include\curl\curl.h`
- `build\vcpkg_installed\x64-windows\include\curl\curlver.h`
- `build\vcpkg_installed\x64-windows\include\curl\easy.h`
- `build\vcpkg_installed\x64-windows\include\curl\header.h`
- `build\vcpkg_installed\x64-windows\include\curl\mprintf.h`
- `build\vcpkg_installed\x64-windows\include\curl\multi.h`
- `build\vcpkg_installed\x64-windows\include\curl\options.h`
- `build\vcpkg_installed\x64-windows\include\curl\system.h`
- `build\vcpkg_installed\x64-windows\include\curl\urlapi.h`
- `build\vcpkg_installed\x64-windows\include\curl\websockets.h`

### build\vcpkg_installed\x64-windows\include\fontconfig/

- `build\vcpkg_installed\x64-windows\include\fontconfig\fontconfig.h`

### build\vcpkg_installed\x64-windows\include\freetype/

- `build\vcpkg_installed\x64-windows\include\freetype\freetype.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftbbox.h`
- `build\vcpkg_installed\x64-windows\include\freetype\fterrdef.h`
- `build\vcpkg_installed\x64-windows\include\freetype\fterrors.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftglyph.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftimage.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftmoderr.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftoutln.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftparams.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftsnames.h`
- `build\vcpkg_installed\x64-windows\include\freetype\ftsystem.h`
- `build\vcpkg_installed\x64-windows\include\freetype\fttypes.h`
- `build\vcpkg_installed\x64-windows\include\freetype\tttables.h`

### build\vcpkg_installed\x64-windows\include\freetype\config/

- `build\vcpkg_installed\x64-windows\include\freetype\config\ftconfig.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\ftheader.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\ftoption.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\ftstdlib.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\integer-types.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\mac-support.h`
- `build\vcpkg_installed\x64-windows\include\freetype\config\public-macros.h`

### build\vcpkg_installed\x64-windows\include\git2/

- `build\vcpkg_installed\x64-windows\include\git2\annotated_commit.h`
- `build\vcpkg_installed\x64-windows\include\git2\apply.h`
- `build\vcpkg_installed\x64-windows\include\git2\attr.h`
- `build\vcpkg_installed\x64-windows\include\git2\blame.h`
- `build\vcpkg_installed\x64-windows\include\git2\blob.h`
- `build\vcpkg_installed\x64-windows\include\git2\branch.h`
- `build\vcpkg_installed\x64-windows\include\git2\buffer.h`
- `build\vcpkg_installed\x64-windows\include\git2\cert.h`
- `build\vcpkg_installed\x64-windows\include\git2\checkout.h`
- `build\vcpkg_installed\x64-windows\include\git2\cherrypick.h`
- `build\vcpkg_installed\x64-windows\include\git2\clone.h`
- `build\vcpkg_installed\x64-windows\include\git2\commit.h`
- `build\vcpkg_installed\x64-windows\include\git2\common.h`
- `build\vcpkg_installed\x64-windows\include\git2\config.h`
- `build\vcpkg_installed\x64-windows\include\git2\credential.h`
- `build\vcpkg_installed\x64-windows\include\git2\credential_helpers.h`
- `build\vcpkg_installed\x64-windows\include\git2\deprecated.h`
- `build\vcpkg_installed\x64-windows\include\git2\describe.h`
- `build\vcpkg_installed\x64-windows\include\git2\diff.h`
- `build\vcpkg_installed\x64-windows\include\git2\email.h`
- `build\vcpkg_installed\x64-windows\include\git2\errors.h`
- `build\vcpkg_installed\x64-windows\include\git2\experimental.h`
- `build\vcpkg_installed\x64-windows\include\git2\filter.h`
- `build\vcpkg_installed\x64-windows\include\git2\global.h`
- `build\vcpkg_installed\x64-windows\include\git2\graph.h`
- `build\vcpkg_installed\x64-windows\include\git2\ignore.h`
- `build\vcpkg_installed\x64-windows\include\git2\index.h`
- `build\vcpkg_installed\x64-windows\include\git2\indexer.h`
- `build\vcpkg_installed\x64-windows\include\git2\mailmap.h`
- `build\vcpkg_installed\x64-windows\include\git2\merge.h`
- `build\vcpkg_installed\x64-windows\include\git2\message.h`
- `build\vcpkg_installed\x64-windows\include\git2\net.h`
- `build\vcpkg_installed\x64-windows\include\git2\notes.h`
- `build\vcpkg_installed\x64-windows\include\git2\object.h`
- `build\vcpkg_installed\x64-windows\include\git2\odb.h`
- `build\vcpkg_installed\x64-windows\include\git2\odb_backend.h`
- `build\vcpkg_installed\x64-windows\include\git2\oid.h`
- `build\vcpkg_installed\x64-windows\include\git2\oidarray.h`
- `build\vcpkg_installed\x64-windows\include\git2\pack.h`
- `build\vcpkg_installed\x64-windows\include\git2\patch.h`
- `build\vcpkg_installed\x64-windows\include\git2\pathspec.h`
- `build\vcpkg_installed\x64-windows\include\git2\proxy.h`
- `build\vcpkg_installed\x64-windows\include\git2\rebase.h`
- `build\vcpkg_installed\x64-windows\include\git2\refdb.h`
- `build\vcpkg_installed\x64-windows\include\git2\reflog.h`
- `build\vcpkg_installed\x64-windows\include\git2\refs.h`
- `build\vcpkg_installed\x64-windows\include\git2\refspec.h`
- `build\vcpkg_installed\x64-windows\include\git2\remote.h`
- `build\vcpkg_installed\x64-windows\include\git2\repository.h`
- `build\vcpkg_installed\x64-windows\include\git2\reset.h`
- `build\vcpkg_installed\x64-windows\include\git2\revert.h`
- `build\vcpkg_installed\x64-windows\include\git2\revparse.h`
- `build\vcpkg_installed\x64-windows\include\git2\revwalk.h`
- `build\vcpkg_installed\x64-windows\include\git2\signature.h`
- `build\vcpkg_installed\x64-windows\include\git2\stash.h`
- `build\vcpkg_installed\x64-windows\include\git2\status.h`
- `build\vcpkg_installed\x64-windows\include\git2\strarray.h`
- `build\vcpkg_installed\x64-windows\include\git2\submodule.h`
- `build\vcpkg_installed\x64-windows\include\git2\tag.h`
- `build\vcpkg_installed\x64-windows\include\git2\trace.h`
- `build\vcpkg_installed\x64-windows\include\git2\transaction.h`
- `build\vcpkg_installed\x64-windows\include\git2\transport.h`
- `build\vcpkg_installed\x64-windows\include\git2\tree.h`
- `build\vcpkg_installed\x64-windows\include\git2\types.h`
- `build\vcpkg_installed\x64-windows\include\git2\version.h`
- `build\vcpkg_installed\x64-windows\include\git2\worktree.h`

### build\vcpkg_installed\x64-windows\include\git2\sys/

- `build\vcpkg_installed\x64-windows\include\git2\sys\credential.h`

### build\vcpkg_installed\x64-windows\include\glm/

- `build\vcpkg_installed\x64-windows\include\glm\common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\exponential.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\fwd.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\geometric.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\glm.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\integer.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\mat4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\matrix.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\packing.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\trigonometric.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vec2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vec3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vec4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\vector_relational.hpp`

### build\vcpkg_installed\x64-windows\include\glm\detail/

- `build\vcpkg_installed\x64-windows\include\glm\detail\_fixes.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\_vectorize.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\compute_common.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\compute_vector_relational.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_common.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_exponential.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_geometric.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_integer.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_matrix.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_packing.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_trigonometric.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\func_vector_relational.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\qualifier.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\setup.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_half.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_half.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat2x4.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat3x4.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_mat4x4.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec1.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec1.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec2.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec3.inl`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\detail\type_vec4.inl`

### build\vcpkg_installed\x64-windows\include\glm\ext/

- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double2x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double3x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_double4x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float2x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float3x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\matrix_float4x4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_int_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\scalar_uint_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_bool4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_double4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float2_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float3_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_float4_precision.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_int4_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint2.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint2_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint3.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint3_sized.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint4.hpp`
- `build\vcpkg_installed\x64-windows\include\glm\ext\vector_uint4_sized.hpp`

### build\vcpkg_installed\x64-windows\include\glm\simd/

- `build\vcpkg_installed\x64-windows\include\glm\simd\platform.h`

### build\vcpkg_installed\x64-windows\include\google\protobuf/

- `build\vcpkg_installed\x64-windows\include\google\protobuf\any.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\any.pb.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arena.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arena_impl.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arenastring.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\arenaz_sampler.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\descriptor.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\endian.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\explicitly_constructed.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\extension_set.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\field_mask.pb.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_enum_reflection.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_enum_util.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_message_bases.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_message_reflection.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\generated_message_util.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\has_bits.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\implicit_weak_message.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\inlined_string_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_entry.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_entry_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_field_inl.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_field_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\map_type_handler.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\message.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\message_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\metadata_lite.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\parse_context.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\port.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\port_def.inc`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\port_undef.inc`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\reflection_ops.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\repeated_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\repeated_ptr_field.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\unknown_field_set.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\wire_format.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\wire_format_lite.h`

### build\vcpkg_installed\x64-windows\include\google\protobuf\io/

- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\coded_stream.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\zero_copy_stream.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\zero_copy_stream_impl.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\io\zero_copy_stream_impl_lite.h`

### build\vcpkg_installed\x64-windows\include\google\protobuf\stubs/

- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\callback.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\casts.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\common.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\hash.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\logging.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\macros.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\mutex.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\once.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\platform_macros.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\port.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\status.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\stl_util.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\stringpiece.h`
- `build\vcpkg_installed\x64-windows\include\google\protobuf\stubs\strutil.h`

### build\vcpkg_installed\x64-windows\include\harfbuzz/

- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-blob.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-buffer.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-common.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-deprecated.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-draw.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-face.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-font.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-ft.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-map.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-paint.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-set.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-shape-plan.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-shape.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-style.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-unicode.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb-version.h`
- `build\vcpkg_installed\x64-windows\include\harfbuzz\hb.h`

### build\vcpkg_installed\x64-windows\include\ngspice/

- `build\vcpkg_installed\x64-windows\include\ngspice\config.h`
- `build\vcpkg_installed\x64-windows\include\ngspice\sharedspice.h`

### build\vcpkg_installed\x64-windows\include\nng/

- `build\vcpkg_installed\x64-windows\include\nng\nng.h`

### build\vcpkg_installed\x64-windows\include\nng\protocol\reqrep0/

- `build\vcpkg_installed\x64-windows\include\nng\protocol\reqrep0\rep.h`

### build\vcpkg_installed\x64-windows\include\opencascade/

- `build\vcpkg_installed\x64-windows\include\opencascade\Standard_Version.hxx`

### build\vcpkg_installed\x64-windows\include\python3.11/

- `build\vcpkg_installed\x64-windows\include\python3.11\Python.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\abstract.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\bltinmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\boolobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\bytearrayobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\bytesobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\ceval.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\codecs.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\compile.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\complexobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\descrobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\dictobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\enumobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\exports.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\fileobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\fileutils.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\floatobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\frameobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\genericaliasobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\import.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\intrcheck.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\iterobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\listobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\longobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\memoryobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\methodobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\modsupport.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\moduleobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\object.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\objimpl.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\osmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\patchlevel.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pybuffer.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pycapsule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyconfig.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyerrors.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyframe.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyhash.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pylifecycle.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymacconfig.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymacro.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymath.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pymem.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pyport.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pystate.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pystrcmp.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pystrtod.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pythonrun.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pythread.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\pytypedefs.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\rangeobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\setobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\sliceobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\structseq.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\sysmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\traceback.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\tracemalloc.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\tupleobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\typeslots.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\unicodeobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\warnings.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\weakrefobject.h`

### build\vcpkg_installed\x64-windows\include\python3.11\cpython/

- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\abstract.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\bytearrayobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\bytesobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\cellobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\ceval.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\classobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\code.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\compile.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\complexobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\context.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\descrobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\dictobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\fileobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\fileutils.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\floatobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\frameobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\funcobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\genobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\import.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\initconfig.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\listobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\longintrepr.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\longobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\methodobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\modsupport.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\object.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\objimpl.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\odictobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\picklebufobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyctype.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pydebug.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyerrors.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyfpe.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pyframe.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pylifecycle.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pymem.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pystate.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pythonrun.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pythread.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\pytime.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\setobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\sysmodule.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\traceback.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\tupleobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\unicodeobject.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\warnings.h`
- `build\vcpkg_installed\x64-windows\include\python3.11\cpython\weakrefobject.h`

### build\vcpkg_installed\x64-windows\include\wx/

- `build\vcpkg_installed\x64-windows\include\wx\aboutdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\accel.h`
- `build\vcpkg_installed\x64-windows\include\wx\access.h`
- `build\vcpkg_installed\x64-windows\include\wx\affinematrix2d.h`
- `build\vcpkg_installed\x64-windows\include\wx\affinematrix2dbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\afterstd.h`
- `build\vcpkg_installed\x64-windows\include\wx\any.h`
- `build\vcpkg_installed\x64-windows\include\wx\anybutton.h`
- `build\vcpkg_installed\x64-windows\include\wx\anystr.h`
- `build\vcpkg_installed\x64-windows\include\wx\app.h`
- `build\vcpkg_installed\x64-windows\include\wx\apptrait.h`
- `build\vcpkg_installed\x64-windows\include\wx\archive.h`
- `build\vcpkg_installed\x64-windows\include\wx\arrstr.h`
- `build\vcpkg_installed\x64-windows\include\wx\artprov.h`
- `build\vcpkg_installed\x64-windows\include\wx\atomic.h`
- `build\vcpkg_installed\x64-windows\include\wx\base64.h`
- `build\vcpkg_installed\x64-windows\include\wx\beforestd.h`
- `build\vcpkg_installed\x64-windows\include\wx\bitmap.h`
- `build\vcpkg_installed\x64-windows\include\wx\bmpbndl.h`
- `build\vcpkg_installed\x64-windows\include\wx\bmpbuttn.h`
- `build\vcpkg_installed\x64-windows\include\wx\bmpcbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\bookctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\brush.h`
- `build\vcpkg_installed\x64-windows\include\wx\buffer.h`
- `build\vcpkg_installed\x64-windows\include\wx\build.h`
- `build\vcpkg_installed\x64-windows\include\wx\button.h`
- `build\vcpkg_installed\x64-windows\include\wx\chartype.h`
- `build\vcpkg_installed\x64-windows\include\wx\checkbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\checkeddelete.h`
- `build\vcpkg_installed\x64-windows\include\wx\checklst.h`
- `build\vcpkg_installed\x64-windows\include\wx\chkconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\choicdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\choice.h`
- `build\vcpkg_installed\x64-windows\include\wx\clipbrd.h`
- `build\vcpkg_installed\x64-windows\include\wx\clntdata.h`
- `build\vcpkg_installed\x64-windows\include\wx\cmdargs.h`
- `build\vcpkg_installed\x64-windows\include\wx\colour.h`
- `build\vcpkg_installed\x64-windows\include\wx\combo.h`
- `build\vcpkg_installed\x64-windows\include\wx\combobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\compiler.h`
- `build\vcpkg_installed\x64-windows\include\wx\compositewin.h`
- `build\vcpkg_installed\x64-windows\include\wx\confbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\config.h`
- `build\vcpkg_installed\x64-windows\include\wx\containr.h`
- `build\vcpkg_installed\x64-windows\include\wx\control.h`
- `build\vcpkg_installed\x64-windows\include\wx\convauto.h`
- `build\vcpkg_installed\x64-windows\include\wx\cpp.h`
- `build\vcpkg_installed\x64-windows\include\wx\crt.h`
- `build\vcpkg_installed\x64-windows\include\wx\ctrlsub.h`
- `build\vcpkg_installed\x64-windows\include\wx\cursor.h`
- `build\vcpkg_installed\x64-windows\include\wx\dataobj.h`
- `build\vcpkg_installed\x64-windows\include\wx\dataview.h`
- `build\vcpkg_installed\x64-windows\include\wx\datetime.h`
- `build\vcpkg_installed\x64-windows\include\wx\dc.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcclient.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcmemory.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcprint.h`
- `build\vcpkg_installed\x64-windows\include\wx\dcscreen.h`
- `build\vcpkg_installed\x64-windows\include\wx\debug.h`
- `build\vcpkg_installed\x64-windows\include\wx\defs.h`
- `build\vcpkg_installed\x64-windows\include\wx\dialog.h`
- `build\vcpkg_installed\x64-windows\include\wx\dir.h`
- `build\vcpkg_installed\x64-windows\include\wx\dirdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\display.h`
- `build\vcpkg_installed\x64-windows\include\wx\dlimpexp.h`
- `build\vcpkg_installed\x64-windows\include\wx\dnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\dvrenderers.h`
- `build\vcpkg_installed\x64-windows\include\wx\dynarray.h`
- `build\vcpkg_installed\x64-windows\include\wx\dynlib.h`
- `build\vcpkg_installed\x64-windows\include\wx\editlbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\encconv.h`
- `build\vcpkg_installed\x64-windows\include\wx\event.h`
- `build\vcpkg_installed\x64-windows\include\wx\eventfilter.h`
- `build\vcpkg_installed\x64-windows\include\wx\evtloop.h`
- `build\vcpkg_installed\x64-windows\include\wx\except.h`
- `build\vcpkg_installed\x64-windows\include\wx\fdrepdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\features.h`
- `build\vcpkg_installed\x64-windows\include\wx\ffile.h`
- `build\vcpkg_installed\x64-windows\include\wx\file.h`
- `build\vcpkg_installed\x64-windows\include\wx\fileconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\filedlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\filedlgcustomize.h`
- `build\vcpkg_installed\x64-windows\include\wx\filefn.h`
- `build\vcpkg_installed\x64-windows\include\wx\filehistory.h`
- `build\vcpkg_installed\x64-windows\include\wx\filename.h`
- `build\vcpkg_installed\x64-windows\include\wx\filepicker.h`
- `build\vcpkg_installed\x64-windows\include\wx\filesys.h`
- `build\vcpkg_installed\x64-windows\include\wx\flags.h`
- `build\vcpkg_installed\x64-windows\include\wx\font.h`
- `build\vcpkg_installed\x64-windows\include\wx\fontenc.h`
- `build\vcpkg_installed\x64-windows\include\wx\frame.h`
- `build\vcpkg_installed\x64-windows\include\wx\fs_arc.h`
- `build\vcpkg_installed\x64-windows\include\wx\fs_zip.h`
- `build\vcpkg_installed\x64-windows\include\wx\gauge.h`
- `build\vcpkg_installed\x64-windows\include\wx\gbsizer.h`
- `build\vcpkg_installed\x64-windows\include\wx\gdicmn.h`
- `build\vcpkg_installed\x64-windows\include\wx\gdiobj.h`
- `build\vcpkg_installed\x64-windows\include\wx\geometry.h`
- `build\vcpkg_installed\x64-windows\include\wx\glcanvas.h`
- `build\vcpkg_installed\x64-windows\include\wx\graphics.h`
- `build\vcpkg_installed\x64-windows\include\wx\grid.h`
- `build\vcpkg_installed\x64-windows\include\wx\hash.h`
- `build\vcpkg_installed\x64-windows\include\wx\hashmap.h`
- `build\vcpkg_installed\x64-windows\include\wx\hashset.h`
- `build\vcpkg_installed\x64-windows\include\wx\headercol.h`
- `build\vcpkg_installed\x64-windows\include\wx\headerctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\hyperlink.h`
- `build\vcpkg_installed\x64-windows\include\wx\icon.h`
- `build\vcpkg_installed\x64-windows\include\wx\iconbndl.h`
- `build\vcpkg_installed\x64-windows\include\wx\iconloc.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\image.h`
- `build\vcpkg_installed\x64-windows\include\wx\imaggif.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagiff.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagjpeg.h`
- `build\vcpkg_installed\x64-windows\include\wx\imaglist.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagpcx.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagpng.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagpnm.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagtga.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagtiff.h`
- `build\vcpkg_installed\x64-windows\include\wx\imagxpm.h`
- `build\vcpkg_installed\x64-windows\include\wx\infobar.h`
- `build\vcpkg_installed\x64-windows\include\wx\init.h`
- `build\vcpkg_installed\x64-windows\include\wx\intl.h`
- `build\vcpkg_installed\x64-windows\include\wx\iosfwrap.h`
- `build\vcpkg_installed\x64-windows\include\wx\ioswrap.h`
- `build\vcpkg_installed\x64-windows\include\wx\itemattr.h`
- `build\vcpkg_installed\x64-windows\include\wx\itemid.h`
- `build\vcpkg_installed\x64-windows\include\wx\kbdstate.h`
- `build\vcpkg_installed\x64-windows\include\wx\language.h`
- `build\vcpkg_installed\x64-windows\include\wx\layout.h`
- `build\vcpkg_installed\x64-windows\include\wx\list.h`
- `build\vcpkg_installed\x64-windows\include\wx\listbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\listbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\listctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\localedefs.h`
- `build\vcpkg_installed\x64-windows\include\wx\log.h`
- `build\vcpkg_installed\x64-windows\include\wx\longlong.h`
- `build\vcpkg_installed\x64-windows\include\wx\math.h`
- `build\vcpkg_installed\x64-windows\include\wx\mdi.h`
- `build\vcpkg_installed\x64-windows\include\wx\memory.h`
- `build\vcpkg_installed\x64-windows\include\wx\menu.h`
- `build\vcpkg_installed\x64-windows\include\wx\menuitem.h`
- `build\vcpkg_installed\x64-windows\include\wx\mimetype.h`
- `build\vcpkg_installed\x64-windows\include\wx\minifram.h`
- `build\vcpkg_installed\x64-windows\include\wx\module.h`
- `build\vcpkg_installed\x64-windows\include\wx\mousestate.h`
- `build\vcpkg_installed\x64-windows\include\wx\msgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msgout.h`
- `build\vcpkg_installed\x64-windows\include\wx\mstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\nonownedwnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\notebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\object.h`
- `build\vcpkg_installed\x64-windows\include\wx\ownerdrw.h`
- `build\vcpkg_installed\x64-windows\include\wx\palette.h`
- `build\vcpkg_installed\x64-windows\include\wx\panel.h`
- `build\vcpkg_installed\x64-windows\include\wx\pen.h`
- `build\vcpkg_installed\x64-windows\include\wx\peninfobase.h`
- `build\vcpkg_installed\x64-windows\include\wx\pickerbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\platform.h`
- `build\vcpkg_installed\x64-windows\include\wx\platinfo.h`
- `build\vcpkg_installed\x64-windows\include\wx\popupwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\process.h`
- `build\vcpkg_installed\x64-windows\include\wx\radiobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\radiobut.h`
- `build\vcpkg_installed\x64-windows\include\wx\range.h`
- `build\vcpkg_installed\x64-windows\include\wx\rawbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\recguard.h`
- `build\vcpkg_installed\x64-windows\include\wx\regex.h`
- `build\vcpkg_installed\x64-windows\include\wx\region.h`
- `build\vcpkg_installed\x64-windows\include\wx\renderer.h`
- `build\vcpkg_installed\x64-windows\include\wx\richmsgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\rtti.h`
- `build\vcpkg_installed\x64-windows\include\wx\sckaddr.h`
- `build\vcpkg_installed\x64-windows\include\wx\scopedptr.h`
- `build\vcpkg_installed\x64-windows\include\wx\scopeguard.h`
- `build\vcpkg_installed\x64-windows\include\wx\scrolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\scrolwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\settings.h`
- `build\vcpkg_installed\x64-windows\include\wx\setup.h`
- `build\vcpkg_installed\x64-windows\include\wx\sharedptr.h`
- `build\vcpkg_installed\x64-windows\include\wx\sizer.h`
- `build\vcpkg_installed\x64-windows\include\wx\slider.h`
- `build\vcpkg_installed\x64-windows\include\wx\snglinst.h`
- `build\vcpkg_installed\x64-windows\include\wx\socket.h`
- `build\vcpkg_installed\x64-windows\include\wx\spinbutt.h`
- `build\vcpkg_installed\x64-windows\include\wx\spinctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\splash.h`
- `build\vcpkg_installed\x64-windows\include\wx\srchctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\statbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\statbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\statline.h`
- `build\vcpkg_installed\x64-windows\include\wx\stattext.h`
- `build\vcpkg_installed\x64-windows\include\wx\statusbr.h`
- `build\vcpkg_installed\x64-windows\include\wx\stdpaths.h`
- `build\vcpkg_installed\x64-windows\include\wx\stdstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\stockitem.h`
- `build\vcpkg_installed\x64-windows\include\wx\stopwatch.h`
- `build\vcpkg_installed\x64-windows\include\wx\strconv.h`
- `build\vcpkg_installed\x64-windows\include\wx\stream.h`
- `build\vcpkg_installed\x64-windows\include\wx\string.h`
- `build\vcpkg_installed\x64-windows\include\wx\stringimpl.h`
- `build\vcpkg_installed\x64-windows\include\wx\stringops.h`
- `build\vcpkg_installed\x64-windows\include\wx\strvararg.h`
- `build\vcpkg_installed\x64-windows\include\wx\sysopt.h`
- `build\vcpkg_installed\x64-windows\include\wx\systhemectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\tarstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\tbarbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\textbuf.h`
- `build\vcpkg_installed\x64-windows\include\wx\textctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\textdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\textentry.h`
- `build\vcpkg_installed\x64-windows\include\wx\textfile.h`
- `build\vcpkg_installed\x64-windows\include\wx\thread.h`
- `build\vcpkg_installed\x64-windows\include\wx\time.h`
- `build\vcpkg_installed\x64-windows\include\wx\timer.h`
- `build\vcpkg_installed\x64-windows\include\wx\tokenzr.h`
- `build\vcpkg_installed\x64-windows\include\wx\toolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\tooltip.h`
- `build\vcpkg_installed\x64-windows\include\wx\toplevel.h`
- `build\vcpkg_installed\x64-windows\include\wx\tracker.h`
- `build\vcpkg_installed\x64-windows\include\wx\translation.h`
- `build\vcpkg_installed\x64-windows\include\wx\treebase.h`
- `build\vcpkg_installed\x64-windows\include\wx\treebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\treectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\treelist.h`
- `build\vcpkg_installed\x64-windows\include\wx\txtstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\typeinfo.h`
- `build\vcpkg_installed\x64-windows\include\wx\types.h`
- `build\vcpkg_installed\x64-windows\include\wx\unichar.h`
- `build\vcpkg_installed\x64-windows\include\wx\uri.h`
- `build\vcpkg_installed\x64-windows\include\wx\url.h`
- `build\vcpkg_installed\x64-windows\include\wx\utils.h`
- `build\vcpkg_installed\x64-windows\include\wx\validate.h`
- `build\vcpkg_installed\x64-windows\include\wx\valtext.h`
- `build\vcpkg_installed\x64-windows\include\wx\variant.h`
- `build\vcpkg_installed\x64-windows\include\wx\vector.h`
- `build\vcpkg_installed\x64-windows\include\wx\version.h`
- `build\vcpkg_installed\x64-windows\include\wx\versioninfo.h`
- `build\vcpkg_installed\x64-windows\include\wx\vidmode.h`
- `build\vcpkg_installed\x64-windows\include\wx\weakref.h`
- `build\vcpkg_installed\x64-windows\include\wx\wfstream.h`
- `build\vcpkg_installed\x64-windows\include\wx\window.h`
- `build\vcpkg_installed\x64-windows\include\wx\windowid.h`
- `build\vcpkg_installed\x64-windows\include\wx\withimages.h`
- `build\vcpkg_installed\x64-windows\include\wx\wupdlock.h`
- `build\vcpkg_installed\x64-windows\include\wx\wx.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxcrt.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxcrtbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\wxcrtvararg.h`
- `build\vcpkg_installed\x64-windows\include\wx\xti.h`
- `build\vcpkg_installed\x64-windows\include\wx\xti2.h`
- `build\vcpkg_installed\x64-windows\include\wx\zipstrm.h`
- `build\vcpkg_installed\x64-windows\include\wx\zstream.h`

### build\vcpkg_installed\x64-windows\include\wx\aui/

- `build\vcpkg_installed\x64-windows\include\wx\aui\aui.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\auibar.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\auibook.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\barartmsw.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\dockart.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\floatpane.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\framemanager.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\tabart.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\tabartmsw.h`
- `build\vcpkg_installed\x64-windows\include\wx\aui\tabmdi.h`

### build\vcpkg_installed\x64-windows\include\wx\generic/

- `build\vcpkg_installed\x64-windows\include\wx\generic\choicdgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\combo.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dataview.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dvrenderer.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\dvrenderers.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\filepickerg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\grid.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\gridctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\grideditors.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\hyperlink.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\infobar.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\logg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\msgdlgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\richmsgdlgg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\scrolwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\spinctlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\splash.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\srchctlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\generic\textdlgg.h`

### build\vcpkg_installed\x64-windows\include\wx\html/

- `build\vcpkg_installed\x64-windows\include\wx\html\htmlcell.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmldefs.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlfilt.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlpars.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmltag.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\htmlwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\m_templ.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\styleparams.h`
- `build\vcpkg_installed\x64-windows\include\wx\html\winpars.h`

### build\vcpkg_installed\x64-windows\include\wx\meta/

- `build\vcpkg_installed\x64-windows\include\wx\meta\convertible.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\if.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\implicitconversion.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\int2type.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\movable.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\pod.h`
- `build\vcpkg_installed\x64-windows\include\wx\meta\removeref.h`

### build\vcpkg_installed\x64-windows\include\wx\msw/

- `build\vcpkg_installed\x64-windows\include\wx\msw\accel.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\anybutton.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\app.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\apptbase.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\apptrait.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\bitmap.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\bmpbuttn.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\bmpcbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\brush.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\button.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\checkbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\checklst.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\chkconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\choice.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\clipbrd.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\colour.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\combo.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\combobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\control.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ctrlsub.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\cursor.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\dialog.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\dirdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\evtloop.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\evtloopconsole.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\fdrepdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\filedlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\font.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\frame.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\gauge.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\gdiimage.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\glcanvas.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\headerctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\hyperlink.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\icon.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\imaglist.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\init.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\libraries.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\listbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\listctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\mdi.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\menu.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\menuitem.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\minifram.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\msgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\nonownedwnd.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\notebook.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ownerdrawnbutton.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ownerdrw.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\palette.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\panel.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\pen.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\popupwin.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\radiobox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\radiobut.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\regconf.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\region.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\registry.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\richmsgdlg.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\scrolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\slider.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\spinbutt.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\spinctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statbmp.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statbox.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statline.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\stattext.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\statusbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\stdpaths.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\textctrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\textentry.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\toolbar.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\tooltip.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\toplevel.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\treectrl.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\window.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\winundef.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\winver.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\wrapwin.h`

### build\vcpkg_installed\x64-windows\include\wx\msw\ole/

- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\access.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dataform.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dataobj.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dataobj2.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\dropsrc.h`
- `build\vcpkg_installed\x64-windows\include\wx\msw\ole\droptgt.h`

### build\vcpkg_installed\x64-windows\include\wx\propgrid/

- `build\vcpkg_installed\x64-windows\include\wx\propgrid\property.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgrid.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgriddefs.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgridiface.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\propgridpagestate.h`
- `build\vcpkg_installed\x64-windows\include\wx\propgrid\props.h`

### build\vcpkg_installed\x64-windows\include\wx\protocol/

- `build\vcpkg_installed\x64-windows\include\wx\protocol\http.h`
- `build\vcpkg_installed\x64-windows\include\wx\protocol\protocol.h`

### build\vcpkg_installed\x64-windows\include\wx\stc/

- `build\vcpkg_installed\x64-windows\include\wx\stc\stc.h`

### build\vcpkg_installed\x64-windows\include\wx\xrc/

- `build\vcpkg_installed\x64-windows\include\wx\xrc\xmlres.h`
- `build\vcpkg_installed\x64-windows\include\wx\xrc\xmlreshandler.h`

### common/

- `common\design_block.h`
- `common\design_block_info.h`
- `common\design_block_info_impl.h`
- `common\design_block_io.h`

### common\dialog_about/

- `common\dialog_about\aboutinfo.h`
- `common\dialog_about\dialog_about.h`
- `common\dialog_about\dialog_about_base.h`

### common\dialogs/

- `common\dialogs\dialog_color_picker_base.h`
- `common\dialogs\dialog_configure_paths_base.h`
- `common\dialogs\dialog_display_html_text_base.h`
- `common\dialogs\dialog_migrate_settings_base.h`
- `common\dialogs\panel_common_settings_base.h`
- `common\dialogs\panel_data_collection_base.h`
- `common\dialogs\panel_mouse_settings_base.h`
- `common\dialogs\panel_packages_and_updates.h`
- `common\dialogs\panel_packages_and_updates_base.h`
- `common\dialogs\panel_plugin_settings_base.h`

### common\dialogs\git/

- `common\dialogs\git\dialog_git_repository.h`
- `common\dialogs\git\dialog_git_repository_base.h`
- `common\dialogs\git\panel_git_repos.h`
- `common\dialogs\git\panel_git_repos_base.h`

### common\git/

- `common\git\kicad_git_common.h`
- `common\git\kicad_git_errors.h`
- `common\git\kicad_git_memory.h`

### common\settings/

- `common\settings\builtin_color_themes.h`

### include/

- `include\advanced_config.h`
- `include\asset_archive.h`
- `include\background_jobs_monitor.h`
- `include\base_screen.h`
- `include\base_set.h`
- `include\base_units.h`
- `include\bin_mod.h`
- `include\bitmap_base.h`
- `include\bitmap_store.h`
- `include\bitmaps.h`
- `include\board_design_settings.h`
- `include\board_item.h`
- `include\build_version.h`
- `include\callback_gal.h`
- `include\class_draw_panel_gal.h`
- `include\collector.h`
- `include\commit.h`
- `include\common.h`
- `include\config_params.h`
- `include\confirm.h`
- `include\ctl_flags.h`
- `include\design_block_lib_table.h`
- `include\dialog_shim.h`
- `include\dpi_scaling.h`
- `include\dpi_scaling_common.h`
- `include\dsnlexer.h`
- `include\eda_base_frame.h`
- `include\eda_dde.h`
- `include\eda_doc.h`
- `include\eda_draw_frame.h`
- `include\eda_item.h`
- `include\eda_item_flags.h`
- `include\eda_pattern_match.h`
- `include\eda_search_data.h`
- `include\eda_shape.h`
- `include\eda_text.h`
- `include\eda_units.h`
- `include\embedded_files.h`
- `include\embedded_files_parser.h`
- `include\env_paths.h`
- `include\env_vars.h`
- `include\file_history.h`
- `include\filename_resolver.h`
- `include\fp_lib_table.h`
- `include\frame_type.h`
- `include\gal_display_options_common.h`
- `include\gestfich.h`
- `include\gr_basic.h`
- `include\gr_text.h`
- `include\grid_tricks.h`
- `include\hashtables.h`
- `include\hotkey_store.h`
- `include\hotkeys_basic.h`
- `include\i18n_utility.h`
- `include\id.h`
- `include\import_export.h`
- `include\inspectable.h`
- `include\json_common.h`
- `include\json_conversions.h`
- `include\json_schema_validator.h`
- `include\ki_any.h`
- `include\ki_exception.h`
- `include\kicommon.h`
- `include\kidialog.h`
- `include\kiface_base.h`
- `include\kiface_ids.h`
- `include\kiid.h`
- `include\kiway.h`
- `include\kiway_express.h`
- `include\kiway_holder.h`
- `include\kiway_player.h`
- `include\launch_ext.h`
- `include\layer_ids.h`
- `include\layer_range.h`
- `include\lib_id.h`
- `include\lib_table_base.h`
- `include\lib_tree_item.h`
- `include\lib_tree_model.h`
- `include\lib_tree_model_adapter.h`
- `include\locale_io.h`
- `include\lockfile.h`
- `include\logging.h`
- `include\lseq.h`
- `include\lset.h`
- `include\macros.h`
- `include\macros_swig.h`
- `include\mail_type.h`
- `include\marker_base.h`
- `include\markup_parser.h`
- `include\netclass.h`
- `include\newstroke_font.h`
- `include\notifications_manager.h`
- `include\origin_transforms.h`
- `include\outline_mode.h`
- `include\page_info.h`
- `include\panel_hotkeys_editor.h`
- `include\paths.h`
- `include\pcb_base_frame.h`
- `include\pcb_display_options.h`
- `include\pcb_screen.h`
- `include\pgm_base.h`
- `include\policy_keys.h`
- `include\progress_reporter.h`
- `include\project.h`
- `include\rc_item.h`
- `include\rc_json_schema.h`
- `include\refdes_utils.h`
- `include\reference_image.h`
- `include\render_settings.h`
- `include\reporter.h`
- `include\richio.h`
- `include\scintilla_tricks.h`
- `include\search_stack.h`
- `include\singleton.h`
- `include\string_any_map.h`
- `include\string_utils.h`
- `include\stroke_params.h`
- `include\stroke_params_parser.h`
- `include\systemdirsappend.h`
- `include\template_fieldnames.h`
- `include\textentry_tricks.h`
- `include\thread_pool.h`
- `include\title_block.h`
- `include\trace_helpers.h`
- `include\ui_events.h`
- `include\undo_redo_container.h`
- `include\units_provider.h`
- `include\validators.h`
- `include\wildcards_and_files_ext.h`
- `include\wx_filename.h`
- `include\wxstream_helper.h`
- `include\zoom_defines.h`

### include\api/

- `include\api\api_enums.h`
- `include\api\api_handler.h`
- `include\api\api_plugin.h`
- `include\api\api_plugin_manager.h`
- `include\api\api_server.h`
- `include\api\api_utils.h`
- `include\api\serializable.h`

### include\bitmaps/

- `include\bitmaps\bitmap_info.h`
- `include\bitmaps\bitmap_types.h`
- `include\bitmaps\bitmaps_list.h`

### include\core/

- `include\core\mirror.h`
- `include\core\typeinfo.h`

### include\database/

- `include\database\database_lib_settings.h`

### include\dialogs/

- `include\dialogs\dialog_color_picker.h`
- `include\dialogs\dialog_configure_paths.h`
- `include\dialogs\dialog_hotkey_list.h`
- `include\dialogs\dialog_migrate_settings.h`
- `include\dialogs\html_message_box.h`
- `include\dialogs\panel_common_settings.h`
- `include\dialogs\panel_data_collection.h`
- `include\dialogs\panel_mouse_settings.h`
- `include\dialogs\panel_plugin_settings.h`

### include\font/

- `include\font\font.h`
- `include\font\fontconfig.h`
- `include\font\fontinfo.h`
- `include\font\glyph.h`
- `include\font\kicad_font_name.h`
- `include\font\outline_decomposer.h`
- `include\font\outline_font.h`
- `include\font\stroke_font.h`
- `include\font\text_attributes.h`
- `include\font\version_info.h`

### include\gal/

- `include\gal\color4d.h`
- `include\gal\cursors.h`
- `include\gal\definitions.h`
- `include\gal\gal.h`
- `include\gal\gal_display_options.h`
- `include\gal\graphics_abstraction_layer.h`
- `include\gal\painter.h`

### include\gal\opengl/

- `include\gal\opengl\gl_context_mgr.h`

### include\io/

- `include\io\io_base.h`
- `include\io\io_mgr.h`

### include\io\kicad/

- `include\io\kicad\kicad_io_utils.h`

### include\kicad_curl/

- `include\kicad_curl\kicad_curl.h`
- `include\kicad_curl\kicad_curl_easy.h`

### include\libeval/

- `include\libeval\numeric_evaluator.h`

### include\libeval_compiler/

- `include\libeval_compiler\libeval_compiler.h`

### include\plotters/

- `include\plotters\plotter.h`

### include\project/

- `include\project\board_project_settings.h`
- `include\project\net_settings.h`
- `include\project\project_archiver.h`
- `include\project\project_file.h`
- `include\project\project_local_settings.h`
- `include\project\sch_project_settings.h`

### include\properties/

- `include\properties\color4d_variant.h`
- `include\properties\eda_angle_variant.h`
- `include\properties\property.h`
- `include\properties\property_mgr.h`
- `include\properties\property_validator.h`
- `include\properties\property_validators.h`
- `include\properties\std_optional_variants.h`

### include\settings/

- `include\settings\app_settings.h`
- `include\settings\aui_settings.h`
- `include\settings\bom_settings.h`
- `include\settings\color_settings.h`
- `include\settings\common_settings.h`
- `include\settings\environment.h`
- `include\settings\grid_settings.h`
- `include\settings\json_settings.h`
- `include\settings\json_settings_internals.h`
- `include\settings\kicad_settings.h`
- `include\settings\layer_settings_utils.h`
- `include\settings\nested_settings.h`
- `include\settings\parameters.h`
- `include\settings\settings_manager.h`

### include\tool/

- `include\tool\action_manager.h`
- `include\tool\action_menu.h`
- `include\tool\action_toolbar.h`
- `include\tool\actions.h`
- `include\tool\common_control.h`
- `include\tool\conditional_menu.h`
- `include\tool\coroutine.h`
- `include\tool\selection.h`
- `include\tool\selection_conditions.h`
- `include\tool\tool_action.h`
- `include\tool\tool_base.h`
- `include\tool\tool_dispatcher.h`
- `include\tool\tool_event.h`
- `include\tool\tool_interactive.h`
- `include\tool\tool_manager.h`
- `include\tool\tool_menu.h`
- `include\tool\tools_holder.h`

### include\view/

- `include\view\view.h`
- `include\view\view_controls.h`
- `include\view\view_group.h`
- `include\view\view_item.h`
- `include\view\view_overlay.h`
- `include\view\view_rtree.h`
- `include\view\wx_view_controls.h`
- `include\view\zoom_controller.h`

### include\widgets/

- `include\widgets\bitmap_button.h`
- `include\widgets\busy_indicator.h`
- `include\widgets\button_row_panel.h`
- `include\widgets\color_swatch.h`
- `include\widgets\filedlg_open_embed_file.h`
- `include\widgets\grid_icon_text_helpers.h`
- `include\widgets\grid_text_button_helpers.h`
- `include\widgets\grid_text_helpers.h`
- `include\widgets\html_window.h`
- `include\widgets\kistatusbar.h`
- `include\widgets\lib_tree.h`
- `include\widgets\msgpanel.h`
- `include\widgets\paged_dialog.h`
- `include\widgets\progress_reporter_base.h`
- `include\widgets\report_severity.h`
- `include\widgets\resettable_panel.h`
- `include\widgets\std_bitmap_button.h`
- `include\widgets\stepped_slider.h`
- `include\widgets\ui_common.h`
- `include\widgets\widget_hotkey_list.h`
- `include\widgets\wx_aui_art_providers.h`
- `include\widgets\wx_busy_indicator.h`
- `include\widgets\wx_dataviewctrl.h`
- `include\widgets\wx_grid.h`
- `include\widgets\wx_grid_autosizer.h`
- `include\widgets\wx_infobar.h`
- `include\widgets\wx_menubar.h`
- `include\widgets\wx_panel.h`
- `include\widgets\wx_splash.h`
- `include\widgets\wx_treebook.h`

### kicad/

- `kicad\kicad_manager_frame.h`

### libs\core\include\core/

- `libs\core\include\core\arraydim.h`
- `libs\core\include\core\ignore.h`
- `libs\core\include\core\json_serializers.h`
- `libs\core\include\core\kicad_algo.h`
- `libs\core\include\core\map_helpers.h`
- `libs\core\include\core\minoptmax.h`
- `libs\core\include\core\observable.h`
- `libs\core\include\core\profile.h`
- `libs\core\include\core\raii.h`
- `libs\core\include\core\spinlock.h`
- `libs\core\include\core\sync_queue.h`
- `libs\core\include\core\utf8.h`
- `libs\core\include\core\wx_stl_compat.h`

### libs\kimath\include/

- `libs\kimath\include\bezier_curves.h`
- `libs\kimath\include\convert_basic_shapes_to_polygon.h`
- `libs\kimath\include\hash.h`
- `libs\kimath\include\hash_128.h`
- `libs\kimath\include\mmh3_hash.h`
- `libs\kimath\include\trigo.h`

### libs\kimath\include\geometry/

- `libs\kimath\include\geometry\approximation.h`
- `libs\kimath\include\geometry\circle.h`
- `libs\kimath\include\geometry\convex_hull.h`
- `libs\kimath\include\geometry\corner_strategy.h`
- `libs\kimath\include\geometry\direction45.h`
- `libs\kimath\include\geometry\eda_angle.h`
- `libs\kimath\include\geometry\ellipse.h`
- `libs\kimath\include\geometry\geometry_utils.h`
- `libs\kimath\include\geometry\half_line.h`
- `libs\kimath\include\geometry\line.h`
- `libs\kimath\include\geometry\point_types.h`
- `libs\kimath\include\geometry\polygon_triangulation.h`
- `libs\kimath\include\geometry\seg.h`
- `libs\kimath\include\geometry\shape.h`
- `libs\kimath\include\geometry\shape_arc.h`
- `libs\kimath\include\geometry\shape_circle.h`
- `libs\kimath\include\geometry\shape_compound.h`
- `libs\kimath\include\geometry\shape_index.h`
- `libs\kimath\include\geometry\shape_line_chain.h`
- `libs\kimath\include\geometry\shape_null.h`
- `libs\kimath\include\geometry\shape_poly_set.h`
- `libs\kimath\include\geometry\shape_rect.h`
- `libs\kimath\include\geometry\shape_segment.h`
- `libs\kimath\include\geometry\shape_simple.h`
- `libs\kimath\include\geometry\shape_utils.h`
- `libs\kimath\include\geometry\vertex_set.h`

### libs\kimath\include\math/

- `libs\kimath\include\math\box2.h`
- `libs\kimath\include\math\box2_minmax.h`
- `libs\kimath\include\math\matrix3x3.h`
- `libs\kimath\include\math\util.h`
- `libs\kimath\include\math\vector2d.h`
- `libs\kimath\include\math\vector2wx.h`
- `libs\kimath\include\math\vector3.h`

### libs\kinng\include/

- `libs\kinng\include\kinng.h`

### libs\kiplatform\include\kiplatform/

- `libs\kiplatform\include\kiplatform\app.h`
- `libs\kiplatform\include\kiplatform\environment.h`
- `libs\kiplatform\include\kiplatform\io.h`
- `libs\kiplatform\include\kiplatform\policy.h`
- `libs\kiplatform\include\kiplatform\secrets.h`
- `libs\kiplatform\include\kiplatform\ui.h`

### pcbnew/

- `pcbnew\board.h`
- `pcbnew\board_commit.h`
- `pcbnew\board_connected_item.h`
- `pcbnew\board_item_container.h`
- `pcbnew\collectors.h`
- `pcbnew\component_class_manager.h`
- `pcbnew\convert_shape_list_to_polygon.h`
- `pcbnew\footprint.h`
- `pcbnew\netinfo.h`
- `pcbnew\pad.h`
- `pcbnew\pad_utils.h`
- `pcbnew\padstack.h`
- `pcbnew\pcb_base_edit_frame.h`
- `pcbnew\pcb_dimension.h`
- `pcbnew\pcb_draw_panel_gal.h`
- `pcbnew\pcb_edit_frame.h`
- `pcbnew\pcb_field.h`
- `pcbnew\pcb_generator.h`
- `pcbnew\pcb_group.h`
- `pcbnew\pcb_item_containers.h`
- `pcbnew\pcb_marker.h`
- `pcbnew\pcb_origin_transforms.h`
- `pcbnew\pcb_painter.h`
- `pcbnew\pcb_plot_params.h`
- `pcbnew\pcb_plot_params_parser.h`
- `pcbnew\pcb_reference_image.h`
- `pcbnew\pcb_shape.h`
- `pcbnew\pcb_table.h`
- `pcbnew\pcb_tablecell.h`
- `pcbnew\pcb_target.h`
- `pcbnew\pcb_text.h`
- `pcbnew\pcb_textbox.h`
- `pcbnew\pcb_track.h`
- `pcbnew\pcb_view.h`
- `pcbnew\pcbexpr_evaluator.h`
- `pcbnew\pcbnew_settings.h`
- `pcbnew\plotprint_opts.h`
- `pcbnew\zone.h`
- `pcbnew\zone_settings.h`
- `pcbnew\zones.h`

### pcbnew\api/

- `pcbnew\api\api_pcb_utils.h`

### pcbnew\board_stackup_manager/

- `pcbnew\board_stackup_manager\board_stackup.h`
- `pcbnew\board_stackup_manager\stackup_predefined_prms.h`

### pcbnew\connectivity/

- `pcbnew\connectivity\connectivity_algo.h`
- `pcbnew\connectivity\connectivity_data.h`
- `pcbnew\connectivity\connectivity_items.h`
- `pcbnew\connectivity\connectivity_rtree.h`
- `pcbnew\connectivity\from_to_cache.h`

### pcbnew\drc/

- `pcbnew\drc\drc_cache_generator.h`
- `pcbnew\drc\drc_creepage_utils.h`
- `pcbnew\drc\drc_engine.h`
- `pcbnew\drc\drc_item.h`
- `pcbnew\drc\drc_rtree.h`
- `pcbnew\drc\drc_rule.h`
- `pcbnew\drc\drc_rule_condition.h`
- `pcbnew\drc\drc_rule_parser.h`
- `pcbnew\drc\drc_test_provider.h`
- `pcbnew\drc\drc_test_provider_clearance_base.h`

### pcbnew\pcb_io/

- `pcbnew\pcb_io\pcb_io.h`
- `pcbnew\pcb_io\pcb_io_mgr.h`

### pcbnew\ratsnest/

- `pcbnew\ratsnest\ratsnest_data.h`

### pcbnew\router/

- `pcbnew\router\pns_algo_base.h`
- `pcbnew\router\pns_debug_decorator.h`
- `pcbnew\router\pns_hole.h`
- `pcbnew\router\pns_item.h`
- `pcbnew\router\pns_itemset.h`
- `pcbnew\router\pns_joint.h`
- `pcbnew\router\pns_layerset.h`
- `pcbnew\router\pns_line.h`
- `pcbnew\router\pns_link_holder.h`
- `pcbnew\router\pns_linked_item.h`
- `pcbnew\router\pns_meander.h`
- `pcbnew\router\pns_meander_placer_base.h`
- `pcbnew\router\pns_node.h`
- `pcbnew\router\pns_placement_algo.h`
- `pcbnew\router\pns_router.h`
- `pcbnew\router\pns_routing_settings.h`
- `pcbnew\router\pns_segment.h`
- `pcbnew\router\pns_sizes_settings.h`
- `pcbnew\router\pns_via.h`
- `pcbnew\router\time_limit.h`

### pcbnew\teardrop/

- `pcbnew\teardrop\teardrop_parameters.h`
- `pcbnew\teardrop\teardrop_types.h`

### pcbnew\tools/

- `pcbnew\tools\pcb_selection.h`

### scripting/

- `scripting\python_manager.h`
- `scripting\python_scripting.h`

### thirdparty\clipper2\Clipper2Lib\include\clipper2/

- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.core.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.engine.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.minkowski.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.offset.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.rectclip.h`
- `thirdparty\clipper2\Clipper2Lib\include\clipper2\clipper.version.h`

### thirdparty\delaunator/

- `thirdparty\delaunator\delaunator.hpp`

### thirdparty\dynamic_bitset/

- `thirdparty\dynamic_bitset\dynamic_bitset.h`

### thirdparty\expected\include\tl/

- `thirdparty\expected\include\tl\expected.hpp`

### thirdparty\fmt\include\fmt/

- `thirdparty\fmt\include\fmt\base.h`
- `thirdparty\fmt\include\fmt\chrono.h`
- `thirdparty\fmt\include\fmt\core.h`
- `thirdparty\fmt\include\fmt\format-inl.h`
- `thirdparty\fmt\include\fmt\format.h`

### thirdparty\json_schema_validator/

- `thirdparty\json_schema_validator\json-patch.hpp`

### thirdparty\json_schema_validator\nlohmann/

- `thirdparty\json_schema_validator\nlohmann\json-schema.hpp`

### thirdparty\libcontext/

- `thirdparty\libcontext\libcontext.h`

### thirdparty\libpopcnt/

- `thirdparty\libpopcnt\libpopcnt.h`

### thirdparty\magic_enum\magic_enum/

- `thirdparty\magic_enum\magic_enum\magic_enum.hpp`

### thirdparty\nlohmann_json\nlohmann/

- `thirdparty\nlohmann_json\nlohmann\json.hpp`
- `thirdparty\nlohmann_json\nlohmann\json_fwd.hpp`

### thirdparty\nlohmann_json\nlohmann\detail/

- `thirdparty\nlohmann_json\nlohmann\detail\abi_macros.hpp`

### thirdparty\other_math/

- `thirdparty\other_math\math_for_graphics.h`

### thirdparty\pegtl/

- `thirdparty\pegtl\pegtl.hpp`

### thirdparty\pegtl\pegtl/

- `thirdparty\pegtl\pegtl\apply_mode.hpp`
- `thirdparty\pegtl\pegtl\argv_input.hpp`
- `thirdparty\pegtl\pegtl\ascii.hpp`
- `thirdparty\pegtl\pegtl\buffer_input.hpp`
- `thirdparty\pegtl\pegtl\change_action.hpp`
- `thirdparty\pegtl\pegtl\change_action_and_state.hpp`
- `thirdparty\pegtl\pegtl\change_action_and_states.hpp`
- `thirdparty\pegtl\pegtl\change_control.hpp`
- `thirdparty\pegtl\pegtl\change_state.hpp`
- `thirdparty\pegtl\pegtl\change_states.hpp`
- `thirdparty\pegtl\pegtl\config.hpp`
- `thirdparty\pegtl\pegtl\cstream_input.hpp`
- `thirdparty\pegtl\pegtl\demangle.hpp`
- `thirdparty\pegtl\pegtl\disable_action.hpp`
- `thirdparty\pegtl\pegtl\discard_input.hpp`
- `thirdparty\pegtl\pegtl\discard_input_on_failure.hpp`
- `thirdparty\pegtl\pegtl\discard_input_on_success.hpp`
- `thirdparty\pegtl\pegtl\enable_action.hpp`
- `thirdparty\pegtl\pegtl\eol.hpp`
- `thirdparty\pegtl\pegtl\file_input.hpp`
- `thirdparty\pegtl\pegtl\istream_input.hpp`
- `thirdparty\pegtl\pegtl\match.hpp`
- `thirdparty\pegtl\pegtl\memory_input.hpp`
- `thirdparty\pegtl\pegtl\mmap_input.hpp`
- `thirdparty\pegtl\pegtl\must_if.hpp`
- `thirdparty\pegtl\pegtl\normal.hpp`
- `thirdparty\pegtl\pegtl\nothing.hpp`
- `thirdparty\pegtl\pegtl\parse.hpp`
- `thirdparty\pegtl\pegtl\parse_error.hpp`
- `thirdparty\pegtl\pegtl\position.hpp`
- `thirdparty\pegtl\pegtl\read_input.hpp`
- `thirdparty\pegtl\pegtl\require_apply.hpp`
- `thirdparty\pegtl\pegtl\require_apply0.hpp`
- `thirdparty\pegtl\pegtl\rewind_mode.hpp`
- `thirdparty\pegtl\pegtl\rules.hpp`
- `thirdparty\pegtl\pegtl\string_input.hpp`
- `thirdparty\pegtl\pegtl\tracking_mode.hpp`
- `thirdparty\pegtl\pegtl\type_list.hpp`
- `thirdparty\pegtl\pegtl\utf8.hpp`
- `thirdparty\pegtl\pegtl\version.hpp`
- `thirdparty\pegtl\pegtl\visit.hpp`

### thirdparty\pegtl\pegtl\contrib/

- `thirdparty\pegtl\pegtl\contrib\parse_tree.hpp`
- `thirdparty\pegtl\pegtl\contrib\remove_first_state.hpp`
- `thirdparty\pegtl\pegtl\contrib\shuffle_states.hpp`

### thirdparty\pegtl\pegtl\internal/

- `thirdparty\pegtl\pegtl\internal\action.hpp`
- `thirdparty\pegtl\pegtl\internal\action_input.hpp`
- `thirdparty\pegtl\pegtl\internal\any.hpp`
- `thirdparty\pegtl\pegtl\internal\apply.hpp`
- `thirdparty\pegtl\pegtl\internal\apply0.hpp`
- `thirdparty\pegtl\pegtl\internal\apply0_single.hpp`
- `thirdparty\pegtl\pegtl\internal\apply_single.hpp`
- `thirdparty\pegtl\pegtl\internal\at.hpp`
- `thirdparty\pegtl\pegtl\internal\bof.hpp`
- `thirdparty\pegtl\pegtl\internal\bol.hpp`
- `thirdparty\pegtl\pegtl\internal\bump.hpp`
- `thirdparty\pegtl\pegtl\internal\bump_help.hpp`
- `thirdparty\pegtl\pegtl\internal\bytes.hpp`
- `thirdparty\pegtl\pegtl\internal\control.hpp`
- `thirdparty\pegtl\pegtl\internal\cr_crlf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\cr_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\crlf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\cstream_reader.hpp`
- `thirdparty\pegtl\pegtl\internal\dependent_false.hpp`
- `thirdparty\pegtl\pegtl\internal\disable.hpp`
- `thirdparty\pegtl\pegtl\internal\discard.hpp`
- `thirdparty\pegtl\pegtl\internal\enable.hpp`
- `thirdparty\pegtl\pegtl\internal\enable_control.hpp`
- `thirdparty\pegtl\pegtl\internal\eof.hpp`
- `thirdparty\pegtl\pegtl\internal\eol.hpp`
- `thirdparty\pegtl\pegtl\internal\eol_pair.hpp`
- `thirdparty\pegtl\pegtl\internal\eolf.hpp`
- `thirdparty\pegtl\pegtl\internal\failure.hpp`
- `thirdparty\pegtl\pegtl\internal\file_mapper_win32.hpp`
- `thirdparty\pegtl\pegtl\internal\file_reader.hpp`
- `thirdparty\pegtl\pegtl\internal\filesystem.hpp`
- `thirdparty\pegtl\pegtl\internal\has_apply.hpp`
- `thirdparty\pegtl\pegtl\internal\has_apply0.hpp`
- `thirdparty\pegtl\pegtl\internal\has_error_message.hpp`
- `thirdparty\pegtl\pegtl\internal\has_match.hpp`
- `thirdparty\pegtl\pegtl\internal\has_unwind.hpp`
- `thirdparty\pegtl\pegtl\internal\identifier.hpp`
- `thirdparty\pegtl\pegtl\internal\if_apply.hpp`
- `thirdparty\pegtl\pegtl\internal\if_must.hpp`
- `thirdparty\pegtl\pegtl\internal\if_must_else.hpp`
- `thirdparty\pegtl\pegtl\internal\if_then_else.hpp`
- `thirdparty\pegtl\pegtl\internal\input_pair.hpp`
- `thirdparty\pegtl\pegtl\internal\istream_reader.hpp`
- `thirdparty\pegtl\pegtl\internal\istring.hpp`
- `thirdparty\pegtl\pegtl\internal\iterator.hpp`
- `thirdparty\pegtl\pegtl\internal\lf_crlf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\lf_eol.hpp`
- `thirdparty\pegtl\pegtl\internal\list.hpp`
- `thirdparty\pegtl\pegtl\internal\list_must.hpp`
- `thirdparty\pegtl\pegtl\internal\list_tail.hpp`
- `thirdparty\pegtl\pegtl\internal\list_tail_pad.hpp`
- `thirdparty\pegtl\pegtl\internal\marker.hpp`
- `thirdparty\pegtl\pegtl\internal\minus.hpp`
- `thirdparty\pegtl\pegtl\internal\missing_apply.hpp`
- `thirdparty\pegtl\pegtl\internal\missing_apply0.hpp`
- `thirdparty\pegtl\pegtl\internal\must.hpp`
- `thirdparty\pegtl\pegtl\internal\not_at.hpp`
- `thirdparty\pegtl\pegtl\internal\one.hpp`
- `thirdparty\pegtl\pegtl\internal\opt.hpp`
- `thirdparty\pegtl\pegtl\internal\pad.hpp`
- `thirdparty\pegtl\pegtl\internal\pad_opt.hpp`
- `thirdparty\pegtl\pegtl\internal\path_to_string.hpp`
- `thirdparty\pegtl\pegtl\internal\peek_char.hpp`
- `thirdparty\pegtl\pegtl\internal\peek_utf8.hpp`
- `thirdparty\pegtl\pegtl\internal\pegtl_string.hpp`
- `thirdparty\pegtl\pegtl\internal\plus.hpp`
- `thirdparty\pegtl\pegtl\internal\raise.hpp`
- `thirdparty\pegtl\pegtl\internal\range.hpp`
- `thirdparty\pegtl\pegtl\internal\ranges.hpp`
- `thirdparty\pegtl\pegtl\internal\rematch.hpp`
- `thirdparty\pegtl\pegtl\internal\rep.hpp`
- `thirdparty\pegtl\pegtl\internal\rep_min.hpp`
- `thirdparty\pegtl\pegtl\internal\rep_min_max.hpp`
- `thirdparty\pegtl\pegtl\internal\rep_opt.hpp`
- `thirdparty\pegtl\pegtl\internal\require.hpp`
- `thirdparty\pegtl\pegtl\internal\result_on_found.hpp`
- `thirdparty\pegtl\pegtl\internal\rules.hpp`
- `thirdparty\pegtl\pegtl\internal\seq.hpp`
- `thirdparty\pegtl\pegtl\internal\sor.hpp`
- `thirdparty\pegtl\pegtl\internal\star.hpp`
- `thirdparty\pegtl\pegtl\internal\star_must.hpp`
- `thirdparty\pegtl\pegtl\internal\state.hpp`
- `thirdparty\pegtl\pegtl\internal\string.hpp`
- `thirdparty\pegtl\pegtl\internal\success.hpp`
- `thirdparty\pegtl\pegtl\internal\try_catch_type.hpp`
- `thirdparty\pegtl\pegtl\internal\until.hpp`

### thirdparty\picosha2/

- `thirdparty\picosha2\picosha2.h`

### thirdparty\potrace\include/

- `thirdparty\potrace\include\auxiliary.h`
- `thirdparty\potrace\include\bitmap.h`
- `thirdparty\potrace\include\bitmap_io.h`
- `thirdparty\potrace\include\curve.h`
- `thirdparty\potrace\include\decompose.h`
- `thirdparty\potrace\include\lists.h`
- `thirdparty\potrace\include\potrace_version.h`
- `thirdparty\potrace\include\potracelib.h`
- `thirdparty\potrace\include\progress.h`
- `thirdparty\potrace\include\trace.h`

### thirdparty\pybind11\include\pybind11/

- `thirdparty\pybind11\include\pybind11\attr.h`
- `thirdparty\pybind11\include\pybind11\buffer_info.h`
- `thirdparty\pybind11\include\pybind11\cast.h`
- `thirdparty\pybind11\include\pybind11\embed.h`
- `thirdparty\pybind11\include\pybind11\eval.h`
- `thirdparty\pybind11\include\pybind11\gil.h`
- `thirdparty\pybind11\include\pybind11\options.h`
- `thirdparty\pybind11\include\pybind11\pybind11.h`
- `thirdparty\pybind11\include\pybind11\pytypes.h`

### thirdparty\pybind11\include\pybind11\detail/

- `thirdparty\pybind11\include\pybind11\detail\class.h`
- `thirdparty\pybind11\include\pybind11\detail\common.h`
- `thirdparty\pybind11\include\pybind11\detail\descr.h`
- `thirdparty\pybind11\include\pybind11\detail\init.h`
- `thirdparty\pybind11\include\pybind11\detail\internals.h`
- `thirdparty\pybind11\include\pybind11\detail\type_caster_base.h`
- `thirdparty\pybind11\include\pybind11\detail\typeid.h`

### thirdparty\rtree\geometry/

- `thirdparty\rtree\geometry\rtree.h`

### thirdparty\thread-pool/

- `thirdparty\thread-pool\bs_thread_pool.hpp`

### 外部路径/

- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_bit_utils.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_chrono.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_filebuf.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_format_ucd_tables.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_formatter.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_heap_algorithms.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_int128.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_iter_core.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_minmax.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_ostream.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_print.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_ranges_to.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_ranges_tuple_formatter.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_sanitizer_annotate_container.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_string_view.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_system_error_abi.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_threads_core.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_tzdb.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\__msvc_xlocinfo_types.hpp`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ammintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\concurrencysal.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\crtdefs.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\eh.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\emmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\excpt.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\immintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\intrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\intrin0.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\intrin0.inl.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\iso646.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\limits.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\mmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\nmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pplcancellation_token.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pplinterface.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ppltasks.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\ppltaskscheduler.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\pplwin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\sal.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\setjmp.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\smmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\stdarg.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\stdbool.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\stdint.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\tmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\use_ansi.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vadefs.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_exception.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_new.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_new_debug.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_startup.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_string.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\vcruntime_typeinfo.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\wmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xatomic.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xatomic_wait.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xbit_ops.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcall_once.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv_ryu.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv_ryu_tables.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xcharconv_tables.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xerrc.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xfilesystem_abi.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xkeycheck.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xmmintrin.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xnode_handle.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xpolymorphic_allocator.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xsmf_control.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xthreads.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\xtimec.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\yvals.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\yvals_core.h`
- `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\zmmintrin.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\WTypesbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\apiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\apisetcconv.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\basetsd.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\bcrypt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\driverspecs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\guiddef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\in6addr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\inaddr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\kernelspecs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\ktmtypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\minwindef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\poppack.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack1.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack4.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\pshpack8.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\qos.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcasync.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcdce.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcdcep.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcndr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcnterr.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\rpcsal.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\sdkddkver.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\sdv_driverspecs.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\specstrings.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\specstrings_strict.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\specstrings_undef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\stralign.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\strsafe.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\tvout.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\winapifamily.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\windef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\winerror.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\winpackagefamily.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\wnnc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\ws2def.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\ws2ipdef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\shared\wtypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\assert.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_io.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_malloc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_math.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_math_defines.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_memcpy_s.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_memory.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_search.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_share.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_startup.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_stdio_config.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_terminate.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wconio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wctype.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wdirect.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wprocess.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wstdio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wstdlib.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wstring.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_wtime.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\crtdbg.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\ctype.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\direct.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\dos.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\errno.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\fcntl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\float.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\inttypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\io.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\locale.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\malloc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\math.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\process.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\share.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\stddef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\stdio.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\stdlib.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\string.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\sys\stat.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\sys\types.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\tchar.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\time.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\wchar.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\ucrt\wctype.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\CommCtrl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\DocObj.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ExDisp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\IsGuids.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\KnownFolders.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ObjectArray.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\PropIdl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\PropIdlBase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShObjIdl_core.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShlDisp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShlGuid.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ShlObj_core.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Shlwapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\SoftPub.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\StructuredQueryCondition.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Unknwn.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Unknwnbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\VersionHelpers.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WS2tcpip.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WerApi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinBase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinNls.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinSock2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinTrust.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\WinUser.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\Windows.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\apiquery2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\cguid.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\combaseapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\comcat.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\coml2api.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\consoleapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\consoleapi2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\consoleapi3.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\datetimeapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\debugapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\dpa_dsa.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\dpapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\enclaveapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\errhandlingapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\fibersapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\fileapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\fileapifromapp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\handleapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\heapapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ime_cmodes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\imm.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\interlockedapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ioapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\jobapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\jobapi2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\libloaderapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\mcx.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\memoryapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\minwinbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\msxml.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\namedpipeapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\namespaceapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ncrypt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\oaidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\objbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\objidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\objidlbase.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ocidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\ole2.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\oleauto.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\oleidl.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\processenv.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\processthreadsapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\processtopologyapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\profileapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propkey.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propkeydef.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propsys.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\propvarutil.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\prsht.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\realtimeapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\reason.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\rpcnsi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\rpcnsip.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\securityappcontainer.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\securitybaseapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\servprov.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\shellapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sherrors.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\shtypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\stringapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\synchapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\sysinfoapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\systemtopologyapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\threadpoolapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\threadpoollegacyapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\timezoneapi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\urlmon.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\utilapiset.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\verrsrc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wincon.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wincontypes.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wincrypt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wingdi.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winhttp.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winnetwk.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winnt.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winreg.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winsvc.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\winver.h`
- `D:\Windows Kits\10\Include\10.0.26100.0\um\wow64apiset.h`

## ⚠️ 未解析符号 (外部依赖)

以下符号需要通过外部库或vcpkg安装包提供：

### Windows/MSVC运行时

- `??_7type_info@@6B@`
- `??_E?$DataHolder@VCOLOR4D@KIGFX@@@?$wxAnyValueTypeOpsGeneric@VCOLOR4D@KIGFX@@@wxPrivate@@UEAAPEAXI@Z`
- `??_E?$DataHolder@VwxDataViewIconText@@@?$wxAnyValueTypeOpsGeneric@VwxDataViewIconText@@@wxPrivate@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@W4PCB_LAYER_ID@@P8BOARD_ITEM@@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_CONNECTED_ITEM@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@W4PCB_LAYER_ID@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VBOARD_ITEM@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_ITEM@@W4KICAD_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@VCOLOR4D@KIGFX@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@VEDA_ANGLE@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@W4LINE_STYLE@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@W4SHAPE_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_SHAPE@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@VCOLOR4D@KIGFX@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@VwxString@@P81@EBAAEBV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@W4GR_TEXT_H_ALIGN_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@W4GR_TEXT_V_ALIGN_T@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VEDA_TEXT@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@V?$optional@H@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@V?$optional@N@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@W4PCB_LAYER_ID@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@W4ZONE_CONNECTION@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VFOOTPRINT@@_NP81@EBA_NXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@HP81@EBAHXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@V?$optional@H@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@V?$optional@N@std@@P81@EBA?AV23@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@VwxString@@P81@EBAAEBV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_ATTRIB@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_DRILL_SHAPE@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_PROP@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4PAD_SHAPE@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4UNCONNECTED_LAYER_MODE@PADSTACK@@P81@EBA?AW423@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPAD@@W4ZONE_CONNECTION@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPCB_DIMENSION_BASE@@NP81@EBANXZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPCB_DIMENSION_BASE@@VwxString@@P81@EBA?AV2@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPCB_DIMENSION_BASE@@W4DIM_ARROW_DIRECTION@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPCB_DIMENSION_BASE@@W4DIM_PRECISION@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- `??_E?$GETTER@VPCB_DIMENSION_BASE@@W4DIM_UNITS_FORMAT@@P81@EBA?AW42@XZ@@UEAAPEAXI@Z`
- ... 还有 446 个符号

### 其他

- `??2@YAPEAX_K@Z`
- `??2@YAPEAX_KAEBUnothrow_t@std@@@Z`
- `??3@YAXPEAX@Z`
- `??3@YAXPEAX_K@Z`

**注意**: 总共有 6641 个未解析符号，此处仅显示前500个

## 🚀 使用建议

### 1. 源文件复制
```bash
# 将最小源文件集合复制到目标项目
# 建议保持相对目录结构
```

### 2. 头文件复制
```bash
# 将头文件闭包复制到目标项目
# 注意保持include路径的一致性
```

### 3. 外部依赖安装
通过vcpkg安装以下包来解决外部符号依赖：
- wxwidgets (如果使用Qt替代，可能不需要)
- boost
- openssl
- sqlite3
- cairo
- 其他根据未解析符号确定的包

## ⚠️ 重要提醒

1. **编译验证**: 此分析基于静态依赖，复制后需要验证编译通过
2. **功能测试**: 最小集合可能缺少运行时动态加载的组件
3. **增量添加**: 如果功能不完整，可以以此为基础逐步添加缺失部分
4. **版本兼容**: 确保外部库版本与KiCad编译环境兼容

---
**报告生成时间**: 1756548699.3456407
**工具**: KiCad依赖分析工具链 (clang-scan-deps + 符号分析)
**目标**: KiCad Qt移植项目最小依赖集合