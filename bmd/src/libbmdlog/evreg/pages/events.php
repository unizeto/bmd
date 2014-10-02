<tr>
	<th colspan="8" align="center"><b><?= $_user ?></b></th>
</tr>
<tr>
	<td colspan="8">
		<table width="100%" border="0" class="hi" align="center" cellpadding="1" cellspacing="1">
			<tr>
				<td style="width:30px" align="center">
					<input type="image" src="pictures/edit_remove.png" onclick="changeAppState(1);">
				</td>
				<td><?= htmlspecialchars($user); ?></td>
				<td style="width:20px"><img src="pictures/edit_user.png" /></td>
			</tr>
		</table>
	</td>
</tr>
<tr>
	<th colspan="8" align="center"><b><?= $events ?></b></th>
</tr>
<tr>
	<td colspan="8">
		<table width="100%" class="hi" border="0" align="center" cellpadding="1" cellspacing="1" >
			<tr>
				<td class="hiLabel1" align="center" style="width: 30px;">
					<input id="colsMenu" type="image" src="pictures/1downarrow1.png" onclick="ShowHide('dbColumns');"/>
				</td>
				<? if ( isset($menu['id']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('log.id')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="right"><b>ID</b></td>
								<td class="hiLabel1" align="right"><? sortColumns('log.id'); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['operation']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('operation_type')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $_opType ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns('operation_type'); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['begin']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('date_time_begin')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $_begin ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("date_time_begin"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['end']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('date_time_end')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $_end ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("date_time_end"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['commit']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('db_date_time_commit')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $dbCommit ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("db_date_time_commit"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['host']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('remote_host')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b>HOST</b></td>
								<td class="hiLabel1" align="right"><? sortColumns("remote_host"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['port']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('remote_port')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b>PORT</b></td>
								<td class="hiLabel1" align="right"><? sortColumns("remote_port"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['pid']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('remote_pid')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b>PID</b></td>
								<td class="hiLabel1" align="right"><? sortColumns("remote_pid"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['level']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('log_level')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $_logLevel ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("log_level"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['string']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('log_string')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $logString ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("log_string"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['reason']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('log_reason')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $logReason ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("log_reason"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['solution']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('log_solution')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $logSolution ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("log_solution"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['service']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('service')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $service ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("service"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['name']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('document_filename')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $docName ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("document_filename"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['size']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('document_size')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b><?= $docSize ?></b></td>
								<td class="hiLabel1" align="right"><? sortColumns("document_size"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<? if ( isset($menu['status']) ) : ?>
					<td class="hiLabel1" onclick="sortOrder('verification_status')">
						<table width="100%" class="hiLabel1" border="0" align="center" cellpadding="1" cellspacing="1" >
							<tr>
								<td class="hiLabel1" align="center"><b>STATUS</b></td>
								<td class="hiLabel1" align="right"><? sortColumns("verification_status"); ?></td>
							</tr>
						</table>
					</td>
				<? endif ?>
				<td class="hiLabel1"></td>
			</tr>
			<tr>
				<td class="default" align="center" style="width: 30px;">
					<div style="position: relative; left: 0px; top: 0;height: 2px;width: 30px;">
						<div id='dbColumns' style="position: relative; left:0px; top: 0px;display: none;">
							<form id='columns' method='POST'>
								<input type="hidden" name="columns[state]" value="-1" />
									<table class="menuEvents" width="160" border="1" cellpadding="0" cellspacing="0">
										<tr>
											<td>
												<input type="checkbox" name ="columns[id]" value="0" <? echo( isset($menu['id']) ? 'checked' : '');?> />
											</td>
											<td width="135">id</td>
										</tr>
										<tr>
											<td>
												<input type="checkbox" name="columns[host]" value="1" <? echo( isset($menu['host']) ? 'checked' : '');?>/>
											</td>
											<td>host</td>
										</tr>
										<tr>
											<td>
												<input type="checkbox" name="columns[port]" value="2" <? echo( isset($menu['port']) ? 'checked' : '');?>/>
											</td>
											<td>port</td>
										</tr>
										<tr>
											<td>
												<input type="checkbox" name="columns[pid]" value="3" <? echo( isset($menu['pid']) ? 'checked' : '');?>/>
											</td>
											<td>pid</td>
										</tr>
										<tr onmouseover="document.getElementById('event').style.display='none';document.getElementById('events').style.display='inline';" onmouseout="document.getElementById('events').style.display='none';document.getElementById('event').style.display='inline';">
											<td></td>
											<td id="event" valign="top"><?= $menuEvent ?></td>
											<td id="events" style="display: none;">
												<table border="0" cellpadding="0" cellspacing="0">
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[begin]" value="5"<? echo( isset($menu['begin']) ? 'checked' : '');?>/><?= $menuEventBegin ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[end]" value="6"<? echo( isset($menu['end']) ? 'checked' : '');?> /><?= $menuEventEnd ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[commit]" value="7"<? echo( isset($menu['commit']) ? 'checked' : '');?>/><?= $menuEventCommit ?></nobr>
														</td>
													</tr>
												</table>
											</td>
										</tr>
										<tr onmouseover="document.getElementById('log').style.display='none';document.getElementById('logs').style.display='inline';" onmouseout="document.getElementById('logs').style.display='none';document.getElementById('log').style.display='inline';">
											<td></td>
											<td id="log" valign="top">log</td>
											<td id="logs" style="display: none;">
												<table border="0" cellpadding="0" cellspacing="0">
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[level]" value="11" <? echo( isset($menu['level']) ? 'checked' : '');?>/><?= $menuLogLevel ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[status]" value="12" <? echo( isset($menu['status']) ? 'checked' : '');?>/><?= $menuLogStatus ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[string]" value="13" <? echo( isset($menu['string']) ? 'checked' : '');?>/><?= $menuLogString ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[reason]" value="14" <? echo( isset($menu['reason']) ? 'checked' : '');?>/><?= $menuLogReason ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[solution]" value="15" <? echo( isset($menu['solution']) ? 'checked' : '');?>/><?= $menuLogSolution ?></nobr>
														</td>
													</tr>
												</table>
											</td>
										</tr>
										<tr>
											<td>
												<input type="checkbox" name="columns[service]" value="16" <? echo( isset($menu['service']) ? 'checked' : '');?> />
											</td>
											<td><?= $menuService ?></td>
										</tr>
										<tr>
											<td>
												<input type="checkbox" name="columns[operation]" value="17" <? echo( isset($menu['operation']) ? 'checked' : '');?> />
											</td>
											<td>
												<nobr><?= $menuOpType ?></nobr>
											</td>
										</tr>
										<tr onmouseover="document.getElementById('document').style.display='none';document.getElementById('documents').style.display='inline';" onmouseout="document.getElementById('documents').style.display='none';document.getElementById('document').style.display='inline';">
											<td></td>
											<td id="document" valign="top">document</td>
											<td id="documents" style="display: none;">
												<table border="0" cellpadding="0" cellspacing="0">
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[size]" value="18" <? echo( isset($menu['size']) ? 'checked' : '');?>/><?= $menuDocSize ?></nobr>
														</td>
													</tr>
													<tr>
														<td>
															<nobr><input type="checkbox" name="columns[name]" value="19" <? echo( isset($menu['name']) ? 'checked' : '');?>/><?= $menuDocName ?></nobr>
														</td>
													</tr>
												</table>
											</td>
										</tr>
								</table>
							</form>
					</div>
			</div>
	</td>
	<td class="default"></td>
</tr>

<?
		$logSet = $dbHandle->query("select * from log inner join (select min(id) as id,count(id) as ac from log where log_owner like '".$user."' group by remote_pid, operation_type,remote_port) grupa on (log.id = grupa.id) order by " . $_SESSION['sortBy'] . " " . $_SESSION['sortDir'], $offset, $limit);
?>

	<? if (count($menu) > 1) : ?>
		<? foreach( $logSet as $log ) : ?>
			<tr>
				<? if ( $log['ac'] > 1 ) : ?>
					<td style="width:30px" align="center">
						<input type="image" src="pictures/edit_add.png" title="<?= $eventsTitle ?>" onclick="changeAppState(3, '<?= $user ?>', '<?= $log['operation_type']; ?>', '<?= $log['remote_pid']; ?>', '<?= $log['remote_port']; ?>', '<?= $log['id']; ?>');">
					</td>
				<? else : ?>
					<td style="width:10px"></td>
				<? endif ?>
				<? if ( isset($menu['id']) ) : ?>
					<td align="center"><?= $log['id']; ?></td>
				<? endif ?>
				<? if ( isset($menu['operation']) ) : ?>
					<td><?= htmlspecialchars($log['operation_type']); ?></td>
				<? endif ?>
				<? if ( isset($menu['begin']) ) : ?>
					<td align="center"><?= $log['date_time_begin']; ?></td>
				<? endif ?>
				<? if ( isset($menu['end']) ) : ?>
					<td align="center"><?= $log['date_time_end']; ?></td>
				<? endif ?>
				<? if ( isset($menu['commit']) ) : ?>
					<td align="center"><?= $log['db_date_time_commit']; ?> </td>
				<? endif ?>
				<? if ( isset($menu['host']) ) : ?>
					<td align="center"><?= $log['remote_host']; ?></td>
				<? endif ?>
				<? if ( isset($menu['port']) ) : ?>
					<td align="center"><?= $log['remote_port']; ?></td>
				<? endif ?>
				<? if ( isset($menu['pid']) ) : ?>
					<td align="center"><?= $log['remote_pid']; ?></td>
				<? endif ?>
				<? if ( isset($menu['level']) ) : ?>
					<td align="center"><?= $log['log_level']; ?></td>
				<? endif ?>
				<? if ( isset($menu['string']) ) : ?>
					<td align="center"><?= $log['log_string']; ?></td>
				<? endif ?>
				<? if ( isset($menu['reason']) ) : ?>
					<td align="center"><?= $log['log_reason']; ?></td>
				<? endif ?>
				<? if ( isset($menu['solution']) ) : ?>
					<td align="center"><?= $log['log_solution']; ?></td>
				<? endif ?>
				<? if ( isset($menu['service']) ) : ?>
					<td align="center"><?= $log['service']; ?></td>
				<? endif ?>
				<? if ( isset($menu['name']) ) : ?>
					<td align="center"><?= $log['document_filename']; ?></td>
				<? endif ?>
				<? if ( isset($menu['size']) ) : ?>
					<td align="center"><?= $log['document_size']; ?></td>
				<? endif ?>
				<? if ( isset($menu['status']) ) : ?>
					<?if ( $log['ac'] > 1 ) : ?>
						<td style="width:70px" align="center"></td>
					<? else : ?>
						<? $logStatus = $dbHandle->select("log", "verification_status", "id = " . $log['id'] ); ?>
						<? if ( $logStatus[0]['verification_status'] == '' ) : ?>
							<td style="width:70px" align="center"><img id="img_<?= $log['id']; ?>" src="pictures/status_unknown.png" title="<?= $logStatusUnknown ?>" /></td>
						<? elseif ( $logStatus[0]['verification_status'] == 0 ) : ?>
							<td style="width:70px" align="center">
								<img id="img_<?= $log['id']; ?>" src="pictures/agt_action_success.png" title="<?= $logStatusSuccess ?>" />
							</td>
						<? else : ?>
							<td style="width:70px" align="center">
								<img id="img_<?= $log['id']; ?>" src="pictures/agt_action_fail.png" title="<?= $logStatusFail ?>" />
							</td>
						<? endif ?>
					<? endif ?>
				<? endif ?>
				<? if ( $log['ac'] > 1 ) : ?>
					<td style="width:30px"></td>
				<? else : ?>
					<td style="width:30px" align="center">
						<input type="image" src="pictures/signature.png" title="<?= $logCheck ?>" onclick="ajaxFunction('<?= $log['id']; ?>');">
					</td>
				<? endif ?>
			</tr>
		<? endforeach ?>
	<? endif ?>
<? $dbHandle->__destroy(); ?>