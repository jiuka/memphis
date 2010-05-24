<?xml version="1.0"?>
<api version="1.0">
	<namespace name="Memphis">
		<function name="debug" symbol="memphis_debug">
			<return-type type="void"/>
			<parameters>
				<parameter name="format" type="gchar*"/>
			</parameters>
		</function>
		<function name="debug_get_print_progress" symbol="memphis_debug_get_print_progress">
			<return-type type="gboolean"/>
		</function>
		<function name="debug_set_print_progress" symbol="memphis_debug_set_print_progress">
			<return-type type="void"/>
			<parameters>
				<parameter name="show_progress" type="gboolean"/>
			</parameters>
		</function>
		<function name="info" symbol="memphis_info">
			<return-type type="void"/>
			<parameters>
				<parameter name="format" type="gchar*"/>
			</parameters>
		</function>
		<boxed name="MemphisRule" type-name="MemphisRule" get-type="memphis_rule_get_type">
			<method name="copy" symbol="memphis_rule_copy">
				<return-type type="MemphisRule*"/>
				<parameters>
					<parameter name="rule" type="MemphisRule*"/>
				</parameters>
			</method>
			<method name="free" symbol="memphis_rule_free">
				<return-type type="void"/>
				<parameters>
					<parameter name="rule" type="MemphisRule*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="memphis_rule_new">
				<return-type type="MemphisRule*"/>
			</constructor>
			<field name="keys" type="gchar**"/>
			<field name="values" type="gchar**"/>
			<field name="type" type="MemphisRuleType"/>
			<field name="polygon" type="MemphisRuleAttr*"/>
			<field name="line" type="MemphisRuleAttr*"/>
			<field name="border" type="MemphisRuleAttr*"/>
			<field name="text" type="MemphisRuleAttr*"/>
		</boxed>
		<boxed name="MemphisRuleAttr" type-name="MemphisRuleAttr" get-type="memphis_rule_attr_get_type">
			<method name="copy" symbol="memphis_rule_attr_copy">
				<return-type type="MemphisRuleAttr*"/>
				<parameters>
					<parameter name="attr" type="MemphisRuleAttr*"/>
				</parameters>
			</method>
			<method name="free" symbol="memphis_rule_attr_free">
				<return-type type="void"/>
				<parameters>
					<parameter name="attr" type="MemphisRuleAttr*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="memphis_rule_attr_new">
				<return-type type="MemphisRuleAttr*"/>
			</constructor>
			<field name="z_min" type="guint8"/>
			<field name="z_max" type="guint8"/>
			<field name="color_red" type="guint8"/>
			<field name="color_green" type="guint8"/>
			<field name="color_blue" type="guint8"/>
			<field name="color_alpha" type="guint8"/>
			<field name="style" type="gchar*"/>
			<field name="size" type="gdouble"/>
		</boxed>
		<enum name="MemphisRuleType" type-name="MemphisRuleType" get-type="memphis_rule_type_get_type">
			<member name="MEMPHIS_RULE_TYPE_UNKNOWN" value="0"/>
			<member name="MEMPHIS_RULE_TYPE_NODE" value="1"/>
			<member name="MEMPHIS_RULE_TYPE_WAY" value="2"/>
			<member name="MEMPHIS_RULE_TYPE_RELATION" value="3"/>
		</enum>
		<object name="MemphisDataPool" parent="GObject" type-name="MemphisDataPool" get-type="memphis_data_pool_get_type">
			<constructor name="new" symbol="memphis_data_pool_new">
				<return-type type="MemphisDataPool*"/>
			</constructor>
			<field name="stringChunk" type="GStringChunk*"/>
			<field name="stringTree" type="GTree*"/>
		</object>
		<object name="MemphisMap" parent="GObject" type-name="MemphisMap" get-type="memphis_map_get_type">
			<method name="free" symbol="memphis_map_free">
				<return-type type="void"/>
				<parameters>
					<parameter name="map" type="MemphisMap*"/>
				</parameters>
			</method>
			<method name="get_bounding_box" symbol="memphis_map_get_bounding_box">
				<return-type type="void"/>
				<parameters>
					<parameter name="map" type="MemphisMap*"/>
					<parameter name="minlat" type="gdouble*"/>
					<parameter name="minlon" type="gdouble*"/>
					<parameter name="maxlat" type="gdouble*"/>
					<parameter name="maxlon" type="gdouble*"/>
				</parameters>
			</method>
			<method name="load_from_data" symbol="memphis_map_load_from_data">
				<return-type type="void"/>
				<parameters>
					<parameter name="map" type="MemphisMap*"/>
					<parameter name="data" type="gchar*"/>
					<parameter name="size" type="guint"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="load_from_file" symbol="memphis_map_load_from_file">
				<return-type type="void"/>
				<parameters>
					<parameter name="map" type="MemphisMap*"/>
					<parameter name="filename" type="gchar*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<constructor name="new" symbol="memphis_map_new">
				<return-type type="MemphisMap*"/>
			</constructor>
		</object>
		<object name="MemphisRenderer" parent="GObject" type-name="MemphisRenderer" get-type="memphis_renderer_get_type">
			<method name="draw_png" symbol="memphis_renderer_draw_png">
				<return-type type="void"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="filename" type="gchar*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="draw_tile" symbol="memphis_renderer_draw_tile">
				<return-type type="void"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="cr" type="cairo_t*"/>
					<parameter name="x" type="guint"/>
					<parameter name="y" type="guint"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="free" symbol="memphis_renderer_free">
				<return-type type="void"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
				</parameters>
			</method>
			<method name="get_column_count" symbol="memphis_renderer_get_column_count">
				<return-type type="gint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="get_map" symbol="memphis_renderer_get_map">
				<return-type type="MemphisMap*"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
				</parameters>
			</method>
			<method name="get_max_x_tile" symbol="memphis_renderer_get_max_x_tile">
				<return-type type="gint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="get_max_y_tile" symbol="memphis_renderer_get_max_y_tile">
				<return-type type="gint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="get_min_x_tile" symbol="memphis_renderer_get_min_x_tile">
				<return-type type="gint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="get_min_y_tile" symbol="memphis_renderer_get_min_y_tile">
				<return-type type="gint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="get_resolution" symbol="memphis_renderer_get_resolution">
				<return-type type="guint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
				</parameters>
			</method>
			<method name="get_row_count" symbol="memphis_renderer_get_row_count">
				<return-type type="gint"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<method name="get_rule_set" symbol="memphis_renderer_get_rule_set">
				<return-type type="MemphisRuleSet*"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="memphis_renderer_new">
				<return-type type="MemphisRenderer*"/>
			</constructor>
			<constructor name="new_full" symbol="memphis_renderer_new_full">
				<return-type type="MemphisRenderer*"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="map" type="MemphisMap*"/>
				</parameters>
			</constructor>
			<method name="set_map" symbol="memphis_renderer_set_map">
				<return-type type="void"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="map" type="MemphisMap*"/>
				</parameters>
			</method>
			<method name="set_resolution" symbol="memphis_renderer_set_resolution">
				<return-type type="void"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="resolution" type="guint"/>
				</parameters>
			</method>
			<method name="set_rule_set" symbol="memphis_renderer_set_rule_set">
				<return-type type="void"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="rules" type="MemphisRuleSet*"/>
				</parameters>
			</method>
			<method name="tile_has_data" symbol="memphis_renderer_tile_has_data">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="renderer" type="MemphisRenderer*"/>
					<parameter name="x" type="guint"/>
					<parameter name="y" type="guint"/>
					<parameter name="zoom_level" type="guint"/>
				</parameters>
			</method>
			<property name="map" type="MemphisMap*" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="resolution" type="guint" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="rule-set" type="MemphisRuleSet*" readable="1" writable="1" construct="0" construct-only="0"/>
		</object>
		<object name="MemphisRuleSet" parent="GObject" type-name="MemphisRuleSet" get-type="memphis_rule_set_get_type">
			<method name="free" symbol="memphis_rule_set_free">
				<return-type type="void"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
				</parameters>
			</method>
			<method name="get_bg_color" symbol="memphis_rule_set_get_bg_color">
				<return-type type="void"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="r" type="guint8*"/>
					<parameter name="g" type="guint8*"/>
					<parameter name="b" type="guint8*"/>
					<parameter name="a" type="guint8*"/>
				</parameters>
			</method>
			<method name="get_rule" symbol="memphis_rule_set_get_rule">
				<return-type type="MemphisRule*"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="id" type="gchar*"/>
				</parameters>
			</method>
			<method name="get_rule_ids" symbol="memphis_rule_set_get_rule_ids">
				<return-type type="GList*"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
				</parameters>
			</method>
			<method name="load_from_data" symbol="memphis_rule_set_load_from_data">
				<return-type type="void"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="data" type="gchar*"/>
					<parameter name="size" type="guint"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="load_from_file" symbol="memphis_rule_set_load_from_file">
				<return-type type="void"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="filename" type="gchar*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<constructor name="new" symbol="memphis_rule_set_new">
				<return-type type="MemphisRuleSet*"/>
			</constructor>
			<method name="remove_rule" symbol="memphis_rule_set_remove_rule">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="id" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_bg_color" symbol="memphis_rule_set_set_bg_color">
				<return-type type="void"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="r" type="guint8"/>
					<parameter name="g" type="guint8"/>
					<parameter name="b" type="guint8"/>
					<parameter name="a" type="guint8"/>
				</parameters>
			</method>
			<method name="set_rule" symbol="memphis_rule_set_set_rule">
				<return-type type="void"/>
				<parameters>
					<parameter name="rules" type="MemphisRuleSet*"/>
					<parameter name="rule" type="MemphisRule*"/>
				</parameters>
			</method>
		</object>
	</namespace>
</api>
