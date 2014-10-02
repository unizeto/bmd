<tr>
	<th colspan="8" align="center"><b><?= $_user ?></b></th>
</tr>
<tr>
	<td colspan="8">
		<table width="100%" class="hi" align="center" cellpadding="1" cellspacing="1">
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
	<th colspan="8" align="center"><b><?= $event ?></b></th>
</tr>
<tr>
	<td colspan="8">
		<table width="100%" class="hi" align="center" cellpadding="1" cellspacing="1">
			<tr>
				<td class="hiLabel1" style="width:30px"></td>
				<td class="hiLabel1" align="center"><b><?= $_opType ?></b></td>
				<td class="hiLabel1" align="center"><b><?= $_begin ?></b></td>
				<td class="hiLabel1" style="width:50px;"></td>
			</tr>
			<?
				$logSet = $dbHandle->select("log", "date_time_begin","log_owner like '".$user."' AND operation_type = '".$opType."' AND remote_pid = '".$remotePid."' AND remote_port = '".$remotePort."' AND id = '".$logId."'" );
			?>
			<tr>
				<td style="width:30px;" align="center">
					<input type="image" src="pictures/edit_remove.png" onclick="changeAppState(2, '<?= $user; ?>');">
				</td>
				<td style="width:50%;" ><?= htmlspecialchars($opType); ?></td>
				<td style="width=40%;" align="center"><?= $logSet[0]['date_time_begin']; ?></td>
				<td style="width:50px"></td>
			</tr>
		</table>
	</td>
</tr>
<tr>
	<th colspan="8" align="center"><b><?= $addInfo ?></b></th>
</tr>
<tr>
	<td colspan="8">
		<table width="100%" class="hi" border="0" align="center" cellpadding="1" cellspacing="1" >
			<tr>
				<td class="hiLabel2" align="center" style="width: 30px;">
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
								<table class="menuAdditional" width="160" border="1" cellpadding="0" cellspacing="0">
									<tr>
										<td>
											<input type="checkbox" name ="columns[id]" value="0" <?= ( isset($menu['id']) ? 'checked' : ''); ?> />
										</td>
										<td width="135">id</td>
									</tr>
									<tr>
										<td>
											<input type="checkbox" name="columns[host]" value="1" <?= ( isset($menu['host']) ? 'checked' : '');?>/>
										</td>
										<td>host</td>
									</tr>
									<tr>
										<td>
											<input type="checkbox" name="columns[port]" value="2" <?= ( isset($menu['port']) ? 'checked' : ''); ?>/>
										</td>
										<td>port</td>
									</tr>
									<tr>
										<td>
											<input type="checkbox" name="columns[pid]" value="3" <?= ( isset($menu['pid']) ? 'checked' : ''); ?>/>
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
														<nobr><input type="checkbox" name="columns[begin]" value="5"<?=( isset($menu['begin']) ? 'checked' : ''); ?>/><?= $menuEventBegin ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[end]" value="6"<?= ( isset($menu['end']) ? 'checked' : '');?> /><?= $menuEventEnd ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[commit]" value="7"<?= ( isset($menu['commit']) ? 'checked' : '');?>/><?= $menuEventCommit ?></nobr>
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
														<nobr><input type="checkbox" name="columns[level]" value="11" <?= ( isset($menu['level']) ? 'checked' : '');?>/><?= $menuLogLevel ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[status]" value="12" <?= ( isset($menu['status']) ? 'checked' : '');?>/><?= $menuLogStatus ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[string]" value="13" <?= ( isset($menu['string']) ? 'checked' : '');?>/><?= $menuLogString ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[reason]" value="14" <?= ( isset($menu['reason']) ? 'checked' : '');?>/><?= $menuLogReason ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[solution]" value="15" <?= ( isset($menu['solution']) ? 'checked' : '');?>/><?= $menuLogSolution ?></nobr>
													</td>
												</tr>
											</table>
										</td>
									</tr>
									<tr>
										<td>
											<input type="checkbox" name="columns[service]" value="16" <?= ( isset($menu['service']) ? 'checked' : '');?> />
										</td>
										<td><?= $menuService ?></td>
									</tr>
									<tr>
										<td>
											<input type="checkbox" name="columns[operation]" value="17" <?= ( isset($menu['operation']) ? 'checked' : '');?> />
										</td>
										<td><nobr><?= $menuOpType ?></nobr></td>
									</tr>
									<tr onmouseover="document.getElementById('document').style.display='none';document.getElementById('documents').style.display='inline';" onmouseout="document.getElementById('documents').style.display='none';document.getElementById('document').style.display='inline';">
										<td></td>
										<td id="document" valign="top">document</td>
										<td id="documents" style="display: none;">
											<table border="0" cellpadding="0" cellspacing="0">
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[size]" value="18" <?= ( isset($menu['size']) ? 'checked' : ''); ?>/><?= $menuDocSize ?></nobr>
													</td>
												</tr>
												<tr>
													<td>
														<nobr><input type="checkbox" name="columns[name]" value="19" <?= ( isset($menu['name']) ? 'checked' : ''); ?>/><?= $menuDocName ?></nobr>
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
					$logSet = $dbHandle->select("log", "*","log_owner like '".$user."' AND operation_type like '".$opType."' AND remote_pid = '".$remotePid."' AND remote_port = '".$remotePort."'", "", $_SESSION['sortBy'] . " " . $_SESSION['sortDir'], $offset, $limit);
				?>

				<? if (!empty($menu)) : ?>
					<? foreach( $logSet as $log ) : ?>
						<tr>
							<td style="width:30px" align="center">
        <img id="imgt_<?= $log['id']; ?>" style="visibility: <?= (($log['verification_status'] != "") && ($log['verification_status'] >= 0)) ? 'visible' : 'hidden' ?>" src="pictures/tree_view.png" onclick="gotoTreeView('<?= $log['id'];?>')" />
       </td>
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
									<td align="center"><?= $log['db_date_time_commit']; ?></td>
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
								<? if ( isset($menu['file']) ) : ?>
									<td align="center"><?= $log['src_file']; ?></td>
								<? endif ?>
								<? if ( isset($menu['line']) ) : ?>
									<td align="center"><?= $log['src_line']; ?></td>
								<? endif ?>
								<? if ( isset($menu['function']) ) : ?>
									<td align="center"><?= $log['src_function']; ?></td>
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
									<? $logStatus = $dbHandle->select("log", "verification_status", "id = " . $log['id'] ); ?>
								<? if ( $logStatus[0]['verification_status'] == '' ) : ?>
									<td style="width:70px" align="center">
										<img id="img_<?= $log['id']; ?>" src="pictures/status_unknown.png" title="<?= $logStatusUnknown ?>" />
									</td>
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
							<td style="width:30px" align="center">
								<input type="image" src="pictures/signature.png" title="<?= $logCheck ?>" onclick="ajaxFunction('<?= $log['id']; ?>');">
							</td>
						</tr>
					<? endforeach ?>
				<? endif ?>
				<? $dbHandle->__destroy(); ?>